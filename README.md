# dtjlp

A basic program to do literate programming, as a literate program! `dtjlp` will take a markdown file and tangle it into source files. It should also weave it into an HTML document, but that part hasn't been done yet.

# Build

To build from the `main` branch, be sure to clone the repo along with its submodules, since it relies on a pre-tangled `dtjlp` submodule to bootstrap itself:

```
$ git clone --recurse-submodules "https://github.com/dylantjohnson/dtjlp.git"
```

The program can be built with CMake.

```
$ cd dtjlp
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

However, if you don't want to use CMake, just checkout one of the release branches instead of `main`, which include the pre-tangled `dtjlp.c` and can be dropped into any C project. There are no dependencies other than C standard library functions.

# Usage

Give `dtjlp` a markdown file and it will spit out source files and documents into the current working directory. Only the code blocks are used when generating source files.

```
$ dtjlp <markdown-file>
```

## Example

Starting with a markdown file, `myLiterateProgram.md`:

````
The main function looks like this:

```c file:hello.c
<<imports>>
int main(void) {
	fprintf(stdout, "hi!\n");
	return 0;
}
```

Now that the important part is explained, here's boring stuff about the imports you need:

```c id:imports
#include <stdio.h>
```
````

A source file is generated, `hello.c`:

```c
#include <stdio.h>
int main(void) {
	fprintf(stdout, "hi!\n");
	return 0;
}
```

And an HTML document is generated, `myLiterateProgram.html`:

```html
<p>The main function looks like this:</p>

<pre><code>
<<imports>>
int main(void) {
	fprintf(stdout, "hi!\n");
	return 0;
}
</code></pre>

<p>Now that the important part is explained, here's boring stuff about the imports you need:</p>

<pre><code>
#include <stdio.h>
</code></pre>
```

## Syntax

Specify what file to output a code block to by annotating it with `file:<filename>`. If a code block is not annotated with the `file` key, then it will be ignored when generating source files. Filenames cannot have spaces, this program is too simple for that.

Give a code block an identifier by annotating it with `id:<code-block-id>`. Giving a code block an identifier is optional, it's only needed if the code block needs to be imported into another. Identifiers cannot have spaces either.

To import code from another code block, reference it with its identifier `<<import-code-block-id>>`. If a code block is being imported, the line cannot have anything else on it. The whole line will be replaced with the imported code block. If the desired identifier is not found, then the line prints as-is. If there are multiple code blocks with the same identifier, whichever that appears first in the document is the one that will be imported.

## Using dtjlp in CMakeLists.txt

If you include `dtjlp` in your project, perhaps as a submodule, it can be used to generate source files automatically as part of the CMake build configuration. `dtjlp` defines a convenient CMake macro, `add_tangle_source`.

In your `CMakeLists.txt`:

```cmake
# Include dtjlp to get access to add_tangle_source macro.
add_subdirectory(dtjlp)

# myLiterateProgram executable depends on myLiterateProgram.c, but myLiterateProgram.c does not exist initially.
add_executable(myLiterateProgram myLiterateProgram.c)

# Inform CMake that myLiterateProgram.c depends on myLiterateProgram.md and can generate the source file by tangling it.
add_tangle_source(
	TARGETS myLiterateProgram.c
	MARKDOWN_FILE myLiterateProgram.md
)
```
