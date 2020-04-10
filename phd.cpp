#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#define NOT_FOUND -1


/* _____ ____  _   _ ______ _____ _____ 
  / ____/ __ \| \ | |  ____|_   _/ ____|
 | |   | |  | |  \| | |__    | || |  __ 
 | |   | |  | | . ` |  __|   | || | |_ |
 | |___| |__| | |\  | |     _| || |__| |
  \_____\____/|_| \_|_|    |_____\_____|*/

string config(const string &inputfn)
{
	//==============================================================
	//* title of your HTML document/blog post
	//* (default: name of .md file)
	const string TITLE = inputfn.substr(0, inputfn.length() - 3);
	//==============================================================
	//* path to the folder containing your images
	//* (default: current folder)
	const string PATH_TO_IMAGE_DIR = ".";
	const string FAVICON = "favicon.ico";
	//==============================================================
	//* path to the folder containing your css
	//* (default: current folder)
	const string PATH_TO_STYLES_DIR = ".";
	const string USR_STYLESHEET = "styles.css";
	//==============================================================
	//* enable if using highlight.js for syntax highlighting
	const bool USING_HIGHLIGHT_JS = true;
	//* path to folder with higlight.js paraphernalia 
	const string PATH_TO_HIGHLIGHT_JS = "./highlight";
	//* one of the styles from highlight.js, in the styles/ dir
	const string SYNTAX_HIGHLIGHT_STYLE = "pojoaque.css";
	//==============================================================

	// DON'T EDIT PAST THIS LINE :)

	string header = "<!DOCTYPE html>\n";
	header += "<html lang=\"en\">\n";
	header += "<head>\n";
	header += "\t<meta charset=\"utf-8\"/>\n";
	header += "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>\n";
	header += "\t<title>" + TITLE + "</title>\n";
	header += "\t<link rel=\"stylesheet\" href=\"" + PATH_TO_STYLES_DIR + "/" + USR_STYLESHEET + "\">\n";
	header += "\t<link rel=\"icon\" href=\"" + PATH_TO_IMAGE_DIR + "/" + FAVICON + "\">\n";

	if(USING_HIGHLIGHT_JS)
	{
		header += "\t<link rel=\"stylesheet\" href=\"" + PATH_TO_HIGHLIGHT_JS + "/styles/" + SYNTAX_HIGHLIGHT_STYLE + "\">\n";
		header += "\t<script src=\"" + PATH_TO_HIGHLIGHT_JS + "/highlight.pack.js\"></script>\n";
		header += "\t<script>hljs.initHighlightingOnLoad();</script>\n";
	}

	header += "</head>\n\n<body>\n";

	return header;
}


/*_____  _____   ____ _______ ____ _________     _______  ______  _____ 
 |  __ \|  __ \ / __ \__   __/ __ \__   __\ \   / /  __ \|  ____|/ ____|
 | |__) | |__) | |  | | | | | |  | | | |   \ \_/ /| |__) | |__  | (___  
 |  ___/|  _  /| |  | | | | | |  | | | |    \   / |  ___/|  __|  \___ \ 
 | |    | | \ \| |__| | | | | |__| | | |     | |  | |    | |____ ____) |
 |_|    |_|  \_\\____/  |_|  \____/  |_|     |_|  |_|    |______|_____/ */

//main parsing function (calls others)
string parse(string md_line, ifstream &fin, int &consecutive_blank_lines, bool &unclosed_p_tag);

// === assorted parsing functions ===
void parse_emphases_to_html_driver(string &md_line);
void parse_emphases_to_html(string &md_line, const string &md_symbol,
	const string &open_html_tag, const string &closed_html_tag);
void parse_images_to_html(string &md_line);
void parse_urls_to_html(string &md_line);
void parse_inline_code_to_html(string &md_line);
bool parse_blockquotes_to_html(string &md_line, bool inner_blockquote);
void parse_blockquotes_to_html(string &md_line, string &html_result, ifstream &fin);
bool parse_headings_to_html(const string &md_line, string &html_result);
bool parse_codeblock_to_html(string &md_line, string &html_result, ifstream &fin);
bool parse_list_to_html(string &md_line, string &html_result, ifstream &fin);

// === helpers ===
void handle_errors(int argc, char **argv);

bool is_ulist_character(const string& c);
bool is_olist_character(const string& c);
int count_indents(const string& md_line);
void append_tabs(string &html_result, int curr_list_level);

string remove_leading_whitespace(const string &md_line);
void close_open_p_tag(string &html_result, bool &unclosed_p_tag, int consecutive_blank_lines);

string replace(std::string subject, const std::string& search, 
	const std::string& replace);
void replace_in_place(std::string &subject, const std::string& search, 
	const std::string& replace);


/*__  __          _____ _   _ 
 |  \/  |   /\   |_   _| \ | |
 | \  / |  /  \    | | |  \| |
 | |\/| | / /\ \   | | | . ` |
 | |  | |/ ____ \ _| |_| |\  |
 |_|  |_/_/    \_\_____|_| \_|*/

int main(int argc, char **argv)
{
	handle_errors(argc, argv);

	string inputfn = argv[1];
	string outputfn = argv[2];

	const string HEADER = config(inputfn);
	const string TAIL = "\n</body>\n</html>";

	ifstream fin(inputfn);
	if(fin.fail()) {std::cout << "Unable to read file.\n"; exit(1);}

	ofstream fout(outputfn);
	if(fout.fail()) {std::cout << "Unable to read file.\n"; exit(1);}

	fout << HEADER << endl;

	int consecutive_blank_lines = 0;
	bool unclosed_p_tag = false; //tracks state, if current line is inside unclosed <p>

	string html_line;
	string md_line;
	while(getline(fin, md_line))
	{
		html_line = parse(md_line, fin, consecutive_blank_lines, unclosed_p_tag);
		fout << html_line;

		if(consecutive_blank_lines >= 1 || !unclosed_p_tag)
			fout << endl;
	}

	if(unclosed_p_tag)
		fout << "\n</p>" << endl;

	fout << TAIL;
}


/*_____        _____   _____ _____ _   _  _____ 
 |  __ \ /\   |  __ \ / ____|_   _| \ | |/ ____|
 | |__) /  \  | |__) | (___   | | |  \| | |  __ 
 |  ___/ /\ \ |  _  / \___ \  | | | . ` | | |_ |
 | |  / ____ \| | \ \ ____) |_| |_| |\  | |__| |
 |_| /_/    \_\_|  \_\_____/|_____|_| \_|\_____|*/

string parse(string md_line, ifstream &fin, int &consecutive_blank_lines, bool &unclosed_p_tag)
{
	string html_result = "";

	//horizontal rule
	if(md_line == "---")
	{
		//<hr> tags can't be within <p> tags, so close any open <p>
		close_open_p_tag(html_result, unclosed_p_tag, consecutive_blank_lines);
		return html_result + "<hr>";
	}

	//if the line only consists of <br> tags, don't wrap it in <p> tags
	if(md_line.length() >= 4 && md_line.substr(0,4) == "<br>" && 
		replace(md_line, "<br>", "") == "")
	{
		consecutive_blank_lines = 0;
		return md_line;
	}

	bool is_codeblock = parse_codeblock_to_html(md_line, html_result, fin);
	if(is_codeblock)
	{
		//<pre> tags can't be within <p> tags, so close any open <p>
		close_open_p_tag(html_result, unclosed_p_tag, consecutive_blank_lines);
		return html_result;
	}

	parse_inline_code_to_html(md_line);

	parse_images_to_html(md_line);
	if(consecutive_blank_lines == 0 && md_line.substr(0,4) == "<img")
	{
		//the img tag doesn't begin a new line, so add one
		md_line = "\n<br>\n" + md_line;
	}

	parse_urls_to_html(md_line);

	parse_emphases_to_html_driver(md_line); //bold, italics, and strikethrough

	//headings, except where entire line is made up of #'s
	//purposely placed at bottom cause headings can contain markdown
	bool is_heading = parse_headings_to_html(md_line, html_result);
	if(is_heading)
	{
		//prefer heading tags to be outside of <p> tags
		close_open_p_tag(html_result, unclosed_p_tag, consecutive_blank_lines);
		return html_result;
	}

	bool is_list = parse_list_to_html(md_line, html_result, fin);
	if(is_list)
	{
		//lists aren't phrasing content, close p tag
		close_open_p_tag(html_result, unclosed_p_tag, consecutive_blank_lines);
		return html_result;
	}

	//blockquotes (purposely at the bottom cause they can contain markdown)
	bool is_blockquote = parse_blockquotes_to_html(md_line, false);
	if(is_blockquote)
	{
		//<blockquote> tags can't be within <p> tags, so close any open <p>
		close_open_p_tag(html_result, unclosed_p_tag, consecutive_blank_lines);
		parse_blockquotes_to_html(md_line, html_result, fin);
		return html_result;
	}

	//consider making this a helper function
	if(md_line != "")
	{
		if(consecutive_blank_lines >= 1)
		{
			//there's been a blank md line, so the content should have a new line

			//this is achieved by either closing a <p> tag and starting a new one,
			if(unclosed_p_tag)
				html_result += "</p>\n\n<p>\n";
			//or by simply opening a <p> tag if one isn't already open
			else
			{
				html_result += "<p>\n";
				unclosed_p_tag = true;
			}
			consecutive_blank_lines = 0;
		}
		else
		{
			//continue appending content to the page as usual

			//"normal" text needs to be in some sort of tag, so place content in
			//<p> tag if one isn't already open
			if(!unclosed_p_tag)
			{
				html_result += "<p>\n";
				unclosed_p_tag = true;
			}
		}
		html_result += md_line;
	}
	else
		consecutive_blank_lines++;


	return html_result;
}

void parse_emphases_to_html_driver(string &md_line)
{
	parse_emphases_to_html(md_line, "**", "<b>", "</b>");
	parse_emphases_to_html(md_line, "__", "<b>", "</b>");
	parse_emphases_to_html(md_line, "~~", "<s>", "</s>");
	parse_emphases_to_html(md_line, "*", "<i>", "</i>");
	parse_emphases_to_html(md_line, "_", "<i>", "</i>");
	parse_emphases_to_html(md_line, "==", "<mark>", "</mark>");
	parse_emphases_to_html(md_line, "++", "<ins>", "</ins>");
}

//parses markdown emphasis syntax into html, in place
void parse_emphases_to_html(string &md_line, const string &md_symbol, 
	const string &open_html_tag, const string &close_html_tag)
{
	const int symbol_len = md_symbol.length();

	int open_index;
	int close_index;

	bool line_begins_with_symbol = md_line.substr(0, symbol_len) == md_symbol;

	//while we find ** in the string
	while(line_begins_with_symbol 
		|| ((open_index = md_line.find(md_symbol)) && open_index != NOT_FOUND))
	{
		//there is a **

		//special case where ** are first 2 chars
		if(line_begins_with_symbol)
		{
			open_index = 0;
			//we dont want to enter the loop for this again
			line_begins_with_symbol = false;
		}

		//search for end in rest of the string (rest = indeces after start + 2)
		close_index = md_line.find(md_symbol, open_index + symbol_len);

		if(close_index != NOT_FOUND)
		{
			//there is a corresponding **

			//replace both **'s with open and closed <b> tag
			md_line.replace(close_index, symbol_len, close_html_tag);
			md_line.replace(open_index, symbol_len, open_html_tag);
		}
		else
		{
			//there's no end to the suffering (i.e no closing **, so do nothing)
			return;
		}
	}
}

//parses markdown image syntax into html, in place
void parse_images_to_html(string &md_line)
{
	//syntax: ![alt](url_to_image)
	//i've chosen to harcode strings and values as i think they're easier to read here

	bool line_begins_with_symbol = md_line.substr(0, 2) == "![";

	int open_bracket_index = 0;

	while(line_begins_with_symbol || 
		((open_bracket_index = md_line.find("![")) && open_bracket_index != NOT_FOUND))
	{
		//found an ![

		//special case where ![ are first 2 chars
		if(line_begins_with_symbol)
		{
			open_bracket_index = 0;
			//we dont want to enter the loop for this again
			line_begins_with_symbol = false;
		}

		//find first instance of ]( after [
		int closed_bracket_index = md_line.find("](", open_bracket_index);
		//find first instance of ) after ](
		int closed_paren_index = md_line.find(")", closed_bracket_index);

		if(closed_bracket_index != NOT_FOUND && closed_paren_index != NOT_FOUND)
		{
			//found ](

			//replace ]( with \" src=\" (hence the 2, we're replacing 2 chars, "[(")
			md_line.replace(closed_bracket_index, 2, "\" src=\"");
			//replace ![ with <img alt=\" (hence the 2, we're replacing 2 chars, "[(")
			md_line.replace(open_bracket_index, 2, "<img alt=\"");

			//weird math making up for extra chars we just inserted (html tags longer than md tags)
			//adjusted index = index - md length + html length
			const string open_alt_symbol = "<img alt=\"";
			closed_bracket_index = closed_bracket_index - 2 + open_alt_symbol.length();

			//find ) following the ](
			int closed_paren_index = md_line.find(")", closed_bracket_index);

			//replace ) with \"> (hence the 1, we're replacing 1 char)
			md_line.replace(closed_paren_index, 1, "\">");

			//FOR OPTIONAL INLINE CSS TAG
			int open_pipe_index = md_line.find("|", closed_paren_index);

			if(open_pipe_index != NOT_FOUND)
			{
				//found a |

				int closed_pipe_index = md_line.find("|", open_pipe_index + 1);
				if(closed_pipe_index != NOT_FOUND)
				{
					//found a closing |

					//get user specified styles
					string styles = md_line.substr(open_pipe_index + 1, 
						closed_pipe_index - open_pipe_index - 1);

					//remove styling metadata from content
					md_line.erase(open_pipe_index, closed_pipe_index - open_pipe_index + 1);

					//add styles to html
					int alt = md_line.find("alt", open_bracket_index);
					md_line.insert(alt, "style=\"" + styles + "\" ");
				}
			}
		}
		else return; //no bracket/paren, not a valid image
	}
}

//parses markdown url syntax into html, in place
void parse_urls_to_html(string &md_line)
{
	//syntax: [text](url)
	//i've chosen to harcode strings and values as i think they're easier to read here

	//most of the +1s and +2s are making up for index and length of symbols when getting text
	//e.g. to get [mytext, the index points to [, so we add 1 to the substring to get mytext

	bool line_begins_with_symbol = (md_line[0] == '[');

	int open_bracket_index = 0;

	while(line_begins_with_symbol || 
		((open_bracket_index = md_line.find("[")) && open_bracket_index != NOT_FOUND))
	{
		//found an [

		//special case where [ is first char
		if(line_begins_with_symbol)
		{
			open_bracket_index = 0;
			//we dont want to enter the loop for this again
			line_begins_with_symbol = false;
		}

		//find first instance of ]( after [
		int closed_bracket_index = md_line.find("](", open_bracket_index);
		int closed_paren_index = md_line.find(")", closed_bracket_index);

		if(closed_bracket_index != NOT_FOUND && closed_paren_index != NOT_FOUND)
		{
			//found ]( and )

			//hyperlink text (i.e. [the stuff in here](example.com))
			string text = md_line.substr(open_bracket_index + 1, 
				closed_bracket_index - open_bracket_index - 1);

			string url = md_line.substr(closed_bracket_index + 2, 
				closed_paren_index - closed_bracket_index - 2);

			//replace ]( with </a> (hence the 2, we're replacing 2 chars, "[(")
			md_line.replace(closed_bracket_index, 2, "\">");
			//replace [ with <a href=\" (hence the 1, we're replacing 1 char, "[")
			md_line.replace(open_bracket_index, 1, "<a href=\"");

			//weird math making up for extra chars we just inserted (html tags longer than md tags)
			//adjusted index = index - md length + html length
			const string open_a_symbol = "<a href=\"";
			closed_bracket_index = closed_bracket_index - 2 + open_a_symbol.length();

			//find ) following the ](
			closed_paren_index = md_line.find(")", closed_bracket_index);

			//replace ) with \"> (hence the 1, we're replacing 1 char)
			md_line.replace(closed_paren_index, 1, "</a>");

			//now swap text and url (cause the order is opposite in md and html)

			int text_pos = md_line.find(text, open_bracket_index);
			//replace text with random single char first,
			//cause if url length < text length, operation will leave behind part of text
			md_line.replace(text_pos, text.length(), "_");
			md_line.replace(text_pos, 1, url);

			//search for url after [stuff inside of brackets](
			//currently inside of brackets is url, so open brace + 1 + url length --> after
			int url_pos = md_line.find(url, open_bracket_index + 1 + url.length());
			md_line.replace(url_pos, url.length(), "_");
			md_line.replace(url_pos, 1, text);
		}
		else return; //no paren/bracket, it's not a valid url
	}
}

void parse_inline_code_to_html(string &md_line)
{
	//syntax: `this is all inline code`
	//i've chosen to harcode strings and values as i think they're easier to read here

	bool line_begins_with_symbol = (md_line[0] == '`');

	int open_backtick_index = 0;

	while(line_begins_with_symbol || 
		((open_backtick_index = md_line.find("`")) && open_backtick_index != NOT_FOUND))
	{
		//found a `

		//special case where ` is first char
		if(line_begins_with_symbol)
		{
			open_backtick_index = 0;
			//we dont want to enter the loop for this again
			line_begins_with_symbol = false;
		}

		//find first instance of ` after `
		int closed_backtick_index = md_line.find("`", open_backtick_index + 1);

		if(closed_backtick_index != NOT_FOUND)
		{
			//remove any special characters from code blocks
			for(int i = open_backtick_index + 1; i < closed_backtick_index; ++i)
			{
				if(md_line[i] == '<')
				{
					md_line.replace(i, 1, "&lt;");
					closed_backtick_index += 3; //difference in length of symbols
				}

				else if(md_line[i] == '>')
				{
					md_line.replace(i, 1, "&gt;");
					closed_backtick_index += 3; //difference in length of symbols
				}
			}

			md_line.replace(closed_backtick_index, 1, "</code>");
			md_line.replace(open_backtick_index, 1, "<code class=\"inline-code\">");
		}
		else return; //no closing backtick found
	}
}

bool parse_headings_to_html(const string &md_line, string &html_result)
{
	if(md_line[0] == '#' && replace(md_line, "#", "") != "")
	{
		int hash_count = 0;
		int i = 0;
		char c = md_line[0];

		while(c == '#' && hash_count < 6) //6 because of <h6>
		{
			++hash_count;
			++i;
			c = md_line[i];
		}

		html_result += "<h" + to_string(hash_count) + ">" +
			md_line.substr(i) + "</h" + to_string(hash_count) + ">";

		return true;
	}
	else return false;
}

bool parse_codeblock_to_html(string &md_line, string &html_result, ifstream &fin)
{
	if(md_line.substr(0,3) == "```")
	{
		string language = md_line.substr(3);
		html_result += "<pre><code class=\"" + language + "\">";

		while(getline(fin, md_line))
		{
			if(md_line == "```") //closing backticks
			{
				html_result += "</code></pre>";
				return true;
			}
			else
			{
				replace_in_place(md_line, "&", "&amp;");
				replace_in_place(md_line, "<", "&lt;");
				replace_in_place(md_line, ">", "&gt;");
				html_result += md_line + "\n";
			}
		}

		cout << "Missing a closing code block! (```)\n";
		exit(1);
	}

	return false;
}

bool parse_list_to_html(string &md_line, string &html_result, ifstream &fin)
{
	if(md_line.length() < 2) //all list syntax has at least 2 chars
		return false;

	string open_tag = "", closed_tag = "";

	if(is_ulist_character(md_line.substr(0, 2)))
	{
		open_tag = "<ul>";
		closed_tag = "</ul>";
	}
	else if(is_olist_character(md_line.substr(0, 2)))
	{
		open_tag = "<ol>";
		closed_tag = "</ol>";
	}

	if(open_tag != "")
	{
		//this is a list

		//begin list and append first bullet
		html_result += open_tag + "\n";
		string content = md_line.substr(2); //everything after "* "
		html_result += "\t<li>" + content + "</li>\n";

		int curr_list_level = 0; //change to curr_nest_level
		int indents = 0;

		streampos prev_line = fin.tellg();

		//loop until md_line is no longer part of the list (doesn't start with "* " )
		while(getline(fin, md_line))
		{
			string trimmed_md_line = remove_leading_whitespace(md_line);
			bool is_list = false;

			if(md_line.length() >= 2)
			{
				//different list types can be nested, must recheck for each line

				if(is_ulist_character(trimmed_md_line.substr(0, 2)))
				{
					open_tag = "<ul>";
					closed_tag = "</ul>";
					is_list = true;
				}
				else if(is_olist_character(trimmed_md_line.substr(0, 2)))
				{
					open_tag = "<ol>";
					closed_tag = "</ol>";
					is_list = true;
				}
			}

			if(is_list)
			{
				//line is still part of the list

				prev_line = fin.tellg();

				//create/close nested list depending on num indents in the line
				indents = count_indents(md_line);

				if(indents > curr_list_level)
				{
					append_tabs(html_result, curr_list_level + 1); //for html code indentation
					html_result += open_tag + "\n";
					curr_list_level++;
				}
				else if(indents < curr_list_level)
				{
					append_tabs(html_result, curr_list_level + 1); //for html code indentation
					html_result += closed_tag + "\n";
					curr_list_level--;
				}

				append_tabs(html_result, curr_list_level + 1); //for html code indentation

				content = trimmed_md_line.substr(2); //cut off "* "
				parse_inline_code_to_html(content);
				parse_images_to_html(content);
				parse_urls_to_html(content);
				parse_emphases_to_html_driver(content);
				html_result += "<li>" + content + "</li>\n";
			}
			else
			{
				fin.seekg(prev_line); //rewind file to parse this line again, not as a list
				break;
			}
		}

		//close all open list tags
		for(int i = 0; i < curr_list_level + 1; ++i)
		{
			for(int j = curr_list_level - i; j > 0; --j) //for html code indentation
				html_result += "\t";

			html_result += closed_tag + "\n";
		}

		return true;
	}

	return false;
}

bool parse_blockquotes_to_html(string &md_line, bool inner_blockquote)
{
	if(md_line[0] == '>' && md_line.length() > 1)
	{
		if(!inner_blockquote)
			md_line = "<blockquote>" + md_line.substr(1);
		else
			md_line = md_line.substr(1);

		return true;
	}
	else return false;
}

void parse_blockquotes_to_html(string &md_line, string &html_result, ifstream &fin)
{
	html_result += md_line;

	//consecutive >blockquotes should be in the same <blockquote> tag
	//the following reads in any consecutive blockquotes into the tag

	//save position of the line so we can rewind the file back once we read too far
	streampos prev_line = fin.tellg();

	bool is_blockquote = true;
	while(getline(fin, md_line) && is_blockquote)
	{
		//blockquotes can contain markdown, so do the processing
		parse_inline_code_to_html(md_line);
		parse_images_to_html(md_line);
		parse_urls_to_html(md_line);
		parse_emphases_to_html_driver(md_line);

		is_blockquote = parse_blockquotes_to_html(md_line, true);

		if(!is_blockquote)
		{
			//no more blockquotes, rewind file back 1 line so it can be parsed next time
				fin.seekg(prev_line);
		}
		else
		{
			//this is another blockquote, simply append content to open <blockquote> tag
			prev_line = fin.tellg();
			html_result += md_line;
		}
	}

	html_result += "</blockquote>";
}

/*_    _ ______ _      _____  ______ _____   _____ 
 | |  | |  ____| |    |  __ \|  ____|  __ \ / ____|
 | |__| | |__  | |    | |__) | |__  | |__) | (___  
 |  __  |  __| | |    |  ___/|  __| |  _  / \___ \ 
 | |  | | |____| |____| |    | |____| | \ \ ____) |
 |_|  |_|______|______|_|    |______|_|  \_\_____/ */

//thank you https://stackoverflow.com/a/14678800
string replace(std::string subject, const std::string& search, const std::string& replace)
{
	if(search == "")
		return "";

	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	
	return subject;
}

//thank you https://stackoverflow.com/a/14678800
void replace_in_place(std::string &subject, const std::string& search, const std::string& replace)
{
	if(search == "")
		return;

	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

bool is_ulist_character(const string& c)
{
	return c[1] == ' ' && (c[0] == '*' || c[0] == '+' || c[0] == '-');
}

bool is_olist_character(const string& c)
{
	return c[1] == '.' && (isdigit(c[0]));
}

int count_indents(const string& md_line)
{
	int spaces = 0;
	int tabs = 0;

	bool found_non_space = false;
	int i = 0;
	while(!found_non_space)
	{
		if(md_line[i] == ' ') spaces++;
		else if(md_line[i] == '\t') tabs++;
		else found_non_space = true;

		++i;
	}

	//let's say every 2 spaces == 1 indent
	int indents = (spaces / 2) + tabs;
	return indents;
}

string remove_leading_whitespace(const string &md_line)
{
	int i = 0;
	while(md_line[i] == ' ' || md_line[i] == '\t')
		++i;

	return md_line.substr(i);
}

void append_tabs(string &html_result, int curr_list_level)
{
	for(int i = 0; i < curr_list_level; ++i)
	{
		html_result += "\t";
	}
}

void close_open_p_tag(string &html_result, bool &unclosed_p_tag, int consecutive_blank_lines)
{
	if(unclosed_p_tag)
	{
		html_result = "</p>\n\n" + html_result;
		unclosed_p_tag = false;
	}

	consecutive_blank_lines = 0;
}

void handle_errors(int argc, char **argv)
{
	if(argc != 3)
	{
		cout << "Usage: " << argv[0] << " input.md output.html\n";
		exit(1);
	}
	int l;

	string inputfn = argv[1];
	if((l = inputfn.length()) && (l < 3 || inputfn.substr(l - 3) != ".md"))
	{
		std::cout << "Invalid input file type (must end in .md)\n";
		exit(1);
	}

	string outputfn = argv[2];
	if((l = outputfn.length()) && (l < 5 || outputfn.substr(l - 5) != ".html"))
	{
		std::cout << "Invalid output file type (must end in .html)\n";
		exit(1);
	}
}