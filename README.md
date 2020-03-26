# phd
A half-baked Markdown to HTML parser, great for blog posts.

*No need for Medical Doctors when you have a Doctor of Philosophy on hand :P*

## Install
* `git clone https://github.com/joshnatis/phd`
* `make`
* `vim inject`
  * configure some of the variables in the script
* Optional: install `highlight.js` from [their page](https://highlightjs.org/download/) for syntax highlighting in code blocks. If you choose not to install it, edit the `inject` script to reflect this.

## Usage
* `./a.out input.md output.html`
* `./inject`
  * inserts header and closing tags into html file
  
## Dependencies
a `c++11` compiler, a shell, `make` -- you have these things.

## Current features
* Headers (\<h1> through \<h6>)
* Emphasis (`**` for **bold**, `~~` for ~~strikethrough~~, `__` for *italic*)
  * Notice that the syntax for italics is different than traditional Markdown.
* Code blocks (automatic syntax highlighting if you link the `highlight.js` script)

## Missing features (for now)
* Lists
* Images
* Links
* Inline code
* Blockquotes
* \<hr> (horizontal rule)
* \<br> (line breaks)
* Inline HTML

## Missing features (forever)
* Everything else (*wait... there's not much else*)

## Note From the Author
I didn't use regex, so now I only have 99 problems instead of [2](https://blog.codinghorror.com/regular-expressions-now-you-have-two-problems/).
