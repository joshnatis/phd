# phd
A Markdown to HTML compiler

<img src="https://user-images.githubusercontent.com/31445542/77646031-27847580-6f3a-11ea-8b5c-d5868bf4b6db.png" alt="euripides" title="A cute euripides" width="20%">

> No need for Medical Doctors when you have a Doctor of Philosophy on hand
_- Gangsta Euripides_

## NOTE

This tool is currently a Markdown to HTML converter and part of a static blog generator stuffed in one vehicle. I have been meaning to fix this for a long time but haven't gotten around to it just yet. Here are some TODOs:
- write a (E)BNF grammar for the language I’m implementing (rather than an informal listing of features)
- decouple the blog functionality from the Markdown parsing functionality
  + corollary: allow Markdown parsing portion to be used as a header library
- better error checking
- tests (pass at least most of John Gruber's [Markdown tests](http://daringfireball.net/projects/downloads/MarkdownTest_1.0.zip))
- use less ad-hoc parsing techniques
- become a UNIX filter program (accept from stdin, write to stdout)

## Install
* `git clone https://github.com/joshnatis/phd`
* edit the variables in the `config()` function on top of `phd.cpp` to configure your settings
* `make`
* Optional: install `highlight.js` from [their page](https://highlightjs.org/download/) for syntax highlighting in code blocks. If you choose not to install it, make sure to set the `USING_HIGHLIGHT_JS` flag to false in `phd.cpp`.

## Usage
* `./phd input.md output.html`
  
## Dependencies
`c++11` and `make`

## Current features
* Headings
  * `#`, `##`, `###`, ... `######`
  * \<h1> through \<h6>
* Emphasis
  * `**x**` or `__x__` for **bold**
  * `*x*` or `_x_` for *italic*
  * `~~x~~` for ~~strikethrough~~
  * `==x==` for <mark>highlight</mark>
  * `++x++` for <u>underline</u>
  * `^x^` for superscript
  * `~x~` for subscript
* Code blocks
  * ```` ```[optional language] <newline> ... <newline> ``` ````
  * automatic syntax highlighting if you link the `highlight.js` script
* Inline code
  * `` `This is inline code` ``
  * This surrounds the inline code in `<code>` tags with the class `inline-code`. You can add styles for `.inline-code` to change how it looks.
* Images
  * `![alt](url)`
  * `![alt](url)|inline css, e.g. width: 50%; height: 30%|`
* Links
  * `[link text](https://url.com)`
* \<hr> (horizontal rule)
    * `---`
* Line breaks
    * One or more blank lines between content in the markdown will yield a blank line in the resultant html
    * You can freely use `<br>` tags
* Blockquotes
    * `> this is all a blockquote`
    * Consecutive `>`'s with no blank lines between will all be funneled into a single blockquote
* Lists
    * `* `, `+ `, or `- ` for bulleted lists, `1.` (or any number) for numbered lists
    * To nest lists, include two spaces or a tab before your bullet point
* Escaped characters
    * `\>`, `\&`, `\<`, `\*`, `\_`, `\^`, `\~`, `` \` ``

## Missing features (for now)
* Alternate URL syntax
  + `[https://url.com]`, `<url.com>`, and/or `https://url.com`.
* Nesting different types of lists together is buggy
* Nesting lists in general is buggy because I misunderstood how nesting lists in HTML works
* Inline code gets parsed for tags (it should have no formatting)
* \<a> tags get parsed for tags (vry bad, this ruins links)

## Missing features (forever)
* Everything else (*wait... there's not much else*)

## Note From the Author
I didn't use regex, so now I only have 99 problems instead of [2](https://blog.codinghorror.com/regular-expressions-now-you-have-two-problems/).

## Pictures!
I know you want em...

![Markdown](https://user-images.githubusercontent.com/31445542/77647237-6ddad400-6f3c-11ea-94b8-65900b429386.png)

---

![HTML](https://user-images.githubusercontent.com/31445542/77647239-6ddad400-6f3c-11ea-8dff-2325aea2b5e4.png)

---
An example blog lives [here](https://josh8.com/blog).
