#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

size_t current_line_length(FILE *file) {
	if (feof(file)) {
		return 0;
	}
	fpos_t current_position;
	fgetpos(file, &current_position);
	size_t count = 0;
	int c = fgetc(file);
	while (c != '\n' && c != EOF) {
		++count;
		c = fgetc(file);
	}
	fsetpos(file, &current_position);
	return count;
}

int32_t is_code_delimeter_line(FILE *md_file) {
	if (feof(md_file)) {
		return 0;
	}
	size_t line_length = current_line_length(md_file);
	if (line_length >= 3) {
		fpos_t current_position;
		fgetpos(md_file, &current_position);
		uint32_t backtick_count = 0;
		for (uint32_t i = 0; i < 3; ++i) {
			if (fgetc(md_file) == '`') {
				++backtick_count;
			} else {
				break;
			}
		}
		fsetpos(md_file, &current_position);
		return backtick_count == 3;
	}
	return 0;
}

void seek_to_next_line(FILE *file) {
	int c = fgetc(file);
	size_t count = 0;
	while (c != '\n' && c != EOF) {
		c = fgetc(file);
	}
}

void seek_to_next_code_delimeter(FILE *md_file) {
	int32_t is_code_delimeter = is_code_delimeter_line(md_file);
	while (!feof(md_file) && !is_code_delimeter) {
		seek_to_next_line(md_file);
		is_code_delimeter = is_code_delimeter_line(md_file);
	}
}

size_t string_length(const char *substring) {
	size_t count = 0;
	while (substring != NULL && *substring++) {
		++count;
	}
	return count;
}

int32_t string_equals(const char *a, const char *b) {
	if ((a == NULL && b != NULL) || (a != NULL && b == NULL)) {
		return 0;
	}
	size_t a_length = string_length(a);
	size_t b_length = string_length(b);
	if (a_length != b_length) {
		return 0;
	}
	if (a_length == 0) {
		return 1;
	}
	for (size_t i = 0; i < a_length; ++i) {
		if (a[i] != b[i]) {
			return 0;
		}
	}
	return 1;
}

int32_t seek_to_substring(FILE *file, const char *substring) {
	if (feof(file)) {
		return 0;
	}
	size_t substring_length = string_length(substring);
	size_t line_length = current_line_length(file);
	fpos_t current_position;
	fgetpos(file, &current_position);
	size_t max_position = line_length > substring_length ? line_length-substring_length : 0;
	for (size_t i = 0; i <= max_position; ++i) {
		int32_t found_substring = 1;
		fpos_t substring_position;
		fgetpos(file, &substring_position);
		for (size_t j = 0; j < substring_length; ++j) {
			int c = fgetc(file);
			if (c != substring[j]) {
				found_substring = 0;
				break;
			}
		}
		fsetpos(file, &substring_position);
		if (found_substring) {
			return 1;
		}
		fseek(file, 1, SEEK_CUR);
	}
	fsetpos(file, &current_position);
	return 0;
}

int32_t is_code_import_line(FILE *md_file) {
	if (feof(md_file)) {
		return 0;
	}
	fpos_t current_position;
	fgetpos(md_file, &current_position);
	int32_t contains_start_import = seek_to_substring(md_file, "<<");
	int32_t contains_end_import = seek_to_substring(md_file, ">>");
	int32_t result = contains_start_import && contains_end_import;
	fsetpos(md_file, &current_position);
	return result;
}

const char *get_value_for_key(FILE *md_file, const char *key) {
	if (feof(md_file)) {
		return NULL;
	}
	fpos_t current_position;
	fgetpos(md_file, &current_position);
	size_t key_length = string_length(key);
	char *key_delimeter = (char *)calloc(key_length+2, sizeof(char));
	if (key_delimeter == NULL) {
		const char *message = "failed to allocate memory for key \"%s\"\n";
		fprintf(stderr, message, key);
		return NULL;
	}
	for (size_t i = 0; i < key_length; ++i) {
		key_delimeter[i] = key[i];
	}
	key_delimeter[key_length] = ':';
	if (!seek_to_substring(md_file, key_delimeter)) {
		free(key_delimeter);
		return NULL;
	}
	free(key_delimeter);
	size_t key_delimeter_length = key_length+1;
	fpos_t keyval_position;
	fgetpos(md_file, &keyval_position);
	fseek(md_file, (long)key_delimeter_length, SEEK_CUR);
	size_t value_length = 0;
	int c = fgetc(md_file);
	while (c != '\n' && c != ' ' && c != '\t' && c != EOF) {
		c = fgetc(md_file);
		++value_length;
	}
	fsetpos(md_file, &keyval_position);
	fseek(md_file, (long)key_delimeter_length, SEEK_CUR);
	char *value = NULL;
	if (value_length > 0) {
		value = (char *)calloc(value_length+1, sizeof(char));
		if (value == NULL) {
			const char *message = "failed to allocate memory for for value of key \"%s\"\n";
			fprintf(stderr, message, key);
		} else {
			for (size_t i = 0; i < value_length; ++i) {
				value[i] = fgetc(md_file);
			}
		}
	}
	fsetpos(md_file, &current_position);
	return value;
}

const char *get_code_filename(FILE *md_file) {
	return get_value_for_key(md_file, "file");
}

const char *get_code_id(FILE *md_file) {
	return get_value_for_key(md_file, "id");
}

const char *get_import_id(FILE *md_file) {
	fpos_t current_position;
	fgetpos(md_file, &current_position);
	int32_t contains_start_import = seek_to_substring(md_file, "<<");
	fpos_t start_import_position;
	fgetpos(md_file, &start_import_position);
	int32_t contains_end_import = seek_to_substring(md_file, ">>");
	fpos_t end_import_position;
	fgetpos(md_file, &end_import_position);
	char *id = NULL;
	if (contains_start_import && contains_end_import) {
		fsetpos(md_file, &start_import_position);
		fseek(md_file, 2, SEEK_CUR);
		fpos_t position;
		fgetpos(md_file, &position);
		size_t id_length = 0;
		while (!feof(md_file) && position < end_import_position) {
			if (fgetc(md_file) != EOF) {
				++id_length;
			}
			fgetpos(md_file, &position);
		}
		if (id_length > 0) {
			id = (char *)calloc(id_length+1, sizeof(char));
			if (id != NULL) {
				fsetpos(md_file, &start_import_position);
				fseek(md_file, 2, SEEK_CUR);
				for (int i = 0; i < id_length; ++i) {
					id[i] = fgetc(md_file);
				}
			}
		}
	}
	fsetpos(md_file, &current_position);
	return id;
}

void initialize_target_files(FILE *md_file) {
	seek_to_next_code_delimeter(md_file);
	while (!feof(md_file)) {
		const char *filename = get_code_filename(md_file);
		if (filename != NULL) {
			FILE *file = fopen(filename, "w");
			if (file != NULL) {
				fclose(file);
			} else {
				const char *message = "failed to initialize target file %s\n";
				fprintf(stderr, message, filename);
			}
			free((char *)filename);
		}
		seek_to_next_line(md_file);
		seek_to_next_code_delimeter(md_file);
	}
	rewind(md_file);
}

int32_t find_and_seek_to_code_delimeter(FILE *md_file, const char *id) {
	fpos_t current_position;
	fgetpos(md_file, &current_position);
	rewind(md_file);
	while (!feof(md_file)) {
		if (is_code_delimeter_line(md_file)) {
			const char *current_id = get_code_id(md_file);
			if (current_id != NULL) {
				int32_t found = string_equals(current_id, id);
				free((char *)current_id);
				if (found) {
					return 1;
				}
			}
		}
		seek_to_next_line(md_file);
	}
	fsetpos(md_file, &current_position);
	return 0;
}

void write_code_from_md_to_file(FILE *md_file, FILE *file) {
	while (!feof(md_file) && !is_code_delimeter_line(md_file)) {
		if (is_code_import_line(md_file)) {
			fpos_t current_position;
			fgetpos(md_file, &current_position);
			const char *id = get_import_id(md_file);
			if (id != NULL) {
				int32_t code_exists = find_and_seek_to_code_delimeter(md_file, id);
				free((char *)id);
				if (code_exists) {
					seek_to_next_line(md_file);
					write_code_from_md_to_file(md_file, file);
				}
				fsetpos(md_file, &current_position);
				if (code_exists) {
					seek_to_next_line(md_file);
					continue;
				}
			}
		}
		int c = fgetc(md_file);
		while (c != '\n' && c != EOF) {
			fputc(c, file);
			c = fgetc(md_file);
		}
		if (c == '\n') {
			fputc(c, file);
		}
	}
}

void tangle(FILE *md_file) {
	initialize_target_files(md_file);
	seek_to_next_code_delimeter(md_file);
	while (!feof(md_file) && is_code_delimeter_line(md_file)) {
		const char *filename = get_code_filename(md_file);
		seek_to_next_line(md_file);
		if (filename != NULL) {
			FILE *file = fopen(filename, "a");
			if (file != NULL) {
				write_code_from_md_to_file(md_file, file);
				fclose(file);
			} else {
				const char *message = "failed to open for appending file %s\n";
				fprintf(stderr, message, filename);
			}
			free((char *)filename);
		}
		seek_to_next_code_delimeter(md_file);
		if (!is_code_delimeter_line(md_file)) {
			const char *message = "expected terminating code delimeter line but none found\n";
			fprintf(stderr, message);
		} else {
			seek_to_next_line(md_file);
			seek_to_next_code_delimeter(md_file);
		}
	}
	rewind(md_file);
}

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "no markdown file specified\n");
		return 1;
	}
	char *md_filename = argv[1];
	FILE *md_file = fopen(md_filename, "r");
	if (md_file == NULL) {
		fprintf(stderr, "failed to open %s\n", md_filename);
		return 1;
	}
	tangle(md_file);
	fclose(md_file);
	fprintf(stdout, "finished processing %s\n", md_filename);
	return 0;
}
