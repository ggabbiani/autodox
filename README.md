# OrthoDocs

[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=ggabbiani_orthodocs&metric=alert_status)](https://sonarcloud.io/project/overview?id=ggabbiani_orthodocs)

![Package output example](docs/package.png "Package output example")

OrthoDocs is an API documentation generator and static analysis tool for the OpenSCAD scripting language. For the list of the feature planned and their status see [Project status](#project-status).

## Design

OrthoDocs comes with a complete OpenSCAD language parser, able to recognize all the public global functions, modules and variables found in the source tree, complete with parameters and their default values. Sources can be passed as files, directories or both. Each source file (here referred to as *package*) will produce a corresponding API document.
Even in a completely uncommented source tree, OrthoDocs will create a document with the following items:

* package name;
* package dependency tree (in textual or graphical mode);
* global variables (name and eventual default value);
* global functions signatures (function name, parameters and eventual defaults);
* global modules signature (name, parameters and eventual defaults);
* one optional Table of Contents for all the items documented in the whole source tree;
* any number of dependecy graphs reassuming the system parts correlation in whatever part of the source tree.

[Here](docs/examples/documents/uncommented.md) you can find the produced document from an uncommented [source file](docs/examples/sources/uncommented.scad):

### Annotations

Orthodocs annotations are single line or C style block comments in which the comment start is immediately followed by the `!` character. All the other comments are ignored.

    /*
     * This is a C-style block comment and is ignored by OrthoDocs
     */

    // This is a single line comment ignored as well

    /*!
     * This is a C-style block comment interpreted as a multi line annotation
     */

     //! This is a single line comment interpreted as a single line annotation

[Here](docs/examples/documents/annotated.md) a document example from the same [source file](docs/examples/sources/annotated.scad) enriched with annotations.

## The command line

OrthoDocs is a CLI only utility, the basic invocation is:

    $ orthodocs --src-root «path to source-root directory» --doc-root «path to document-root» «list of files and/or directory relative to source-root»

OrthoDocs will search in all the input directories for any scad file, the resulting file list being merged with the files eventually passed as positionals and analized.
The analysis results will be used for producing a number of Markdown documents (one for each
source file analized).

Assuming the following directory tree:

    /home/
    ┖── giampa/
        ┖── example/
            ┠── docs/
            ┖── sources/
                ┠── 2d.scad
                ┠── annotated.scad
                ┠── bbox.scad
                ┠── type_trait.scad
                ┖── uncommented.scad

the following command executed in the directory «/home/giampa»

    /home/giampa $ orthodocs --src-root example/sources --doc-root example/doc

will scan all the SCAD sources in the source-root and produce the relative documents in the doc-root

    /home/
    ┖── giampa/
        ┖── example/
            ┠── docs/
            ┃   ┠── 2d.md
            ┃   ┠── annotated.md
            ┃   ┠── bbox.md
            ┃   ┠── type_trait.md
            ┃   ┖── uncommented.md
            ┖── sources/
                ┠── 2d.scad
                ┠── annotated.scad
                ┠── bbox.scad
                ┠── type_trait.scad
                ┖── uncommented.scad

If we want to keep the same source-root while documenting only one subset the following command

    /home/giampa $ orthodocs --src-root example/sources --doc-root example/doc annotated.scad uncommented.scad

will produce

    /home/
    ┖── giampa/
        ┖── example/
            ┠── docs/
            ┃   ┠── annotated.md
            ┃   ┖── uncommented.md
            ┖── sources/
                ┠── 2d.scad
                ┠── annotated.scad
                ┠── bbox.scad
                ┠── type_trait.scad
                ┖── uncommented.scad

For more complex tasks this is the full option list:

| Option             | Description
| ------------------ | -----------
| -h,--help          | Print the help message and exit
| -a,--admonitions   | when enabled any admonition found in annotations will be enriched with a corresponding emoj
| -s,--src-root      | source tree root - either an absolute or current directory relative path. It is a **mandatory existing directory** parameter.
| -d,--doc-root      | document tree root - either an absolute or current directory relative path. It is a **mandatory directory** parameter, if not existing is created.
| -t,--toc           | generate a Table of Contents in the document tree root
| -i,--ignore-prefix | ignore this prefix in the Table of Contents when sorting
| --pd,--pkg-deps    | set package dependecies representation by text list or by a dependency graph (possible value **text**/**graph**, default to **text**)
| -g,--graphs        | Needs: --src-root. List of root relative directories where placing dependency graphs
| -p,--private       | Every documentation item (variable, function or module) prefixed with this string will not be published.
  -q,--quiet         | quiet mode, no statistics will be printed after successfully execution.

## Project status

| Feature                   | Status
| ------------------------- | ------
| [admonitions](https://www.markdownguide.org/hacks/#admonitions)| ✔
| comment styles            | ✔
| cross-reference           | 🚧
| dependency graphs         | ✔
| global values             | ✔
| Linux support             | ✔
| macOS support             | ✔
| source license detection  | ✔
| table of contents         | ✔
| Windows support           | 🚧
