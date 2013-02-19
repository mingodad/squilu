class OurHelpWindow extends HelpWindow
{
	_help_file_name = null;
	_navigation = null;
	_last_found_pos = null;
	_help_text_buffer = null;


	constructor() {
		base.constructor();
		_help_file_name = "help.txt";
		_last_found_pos = 0;
		_navigation = [];
		help_text->wrap_mode(1, 0);
		_help_text_buffer = help_text->buffer();
		view_html->textsize(view_html->labelsize());
		view_html->link(hlp_link);
		btnSavehelp.callback(save_help_file);
		btnSearchHelp.callback(on_search);
		words_to_search.callback(on_search);
		btnFontSmaller.callback(on_font_size);
		btnFontBigger.callback(on_font_size);
		btnBackward.callback(on_navigate);
		btnTop.callback(on_navigate);
		btnForward.callback(on_navigate);
		tabs.callback(on_tab_change);
	}

	function hlp_link(sender, uri, anchor){
		this = sender->window();
		if(uri.startswith("db:/")){
			printf("%s\n", uri);
			return null;
		}
		_navigation.push(sender->topline());
		return uri;
	}

	function refresh_html() {
		local html = markdown2html(_help_text_buffer->text(), _help_text_buffer->length());
		view_html->value(html);
		//printf(html.c_str());
	}

	function load_help_file(fn=null){
		fn = fn ? fn : _help_file_name;
		_help_text_buffer->loadfile(fn);
		_help_file_name = fn;
		refresh_html();
	}

	function save_help_file() {
		_help_text_buffer->savefile(_help_file_name);
		btnSavehelp->deactivate();
	}

	function on_search(){
		search_help_file(words_to_search->value());
	}

	function search_help_file(topic, fromStart=false) {
		words_to_search->value(topic);
		if(!topic) return;

		if(!_help_text_buffer->length()) load_help_file();

		if(fromStart || words_to_search->changed2()){
			_last_found_pos =0;
			words_to_search->clear_changed2();
		}

		local foundPos = -1;
		
		if(tabs->value() == tabView){
			foundPos = view_html->find(topic, _last_found_pos);
			if(foundPos >= 0) _last_found_pos = foundPos;
			else foundPos = 0;
		}
		else
		{
			if((foundPos = _help_text_buffer->search_forward(_last_found_pos, topic)) >= 0){
				_help_text_buffer->select(foundPos, foundPos+topic.len());
				help_text->insert_position(foundPos);
				help_text->show_insert_position();
				_last_found_pos = foundPos+1;
				//delayed_focus(help_text);
			}
		}

		if(foundPos < 0){
			_last_found_pos = 0;
			fl_alert(format(_tr("Nothing found for '%s' !"), topic));
		}
	}

	function on_font_size(sender, udata){
		this = sender->window(); //resolve vars on this window class
		
		local fsize = view_html->textsize();
		if(sender == btnFontSmaller){
			if (fsize > 10){
				view_html->textsize(fsize - 2);
				view_html->reformat();
			}

			if (view_html->textsize() <= 10) btnFontSmaller->deactivate();
			btnFontBigger->activate();
		}
		else
		{
			if (fsize <= 24){
				view_html->textsize(fsize + 2);
				view_html->reformat();
			}

			if (view_html->textsize() > 24) btnFontBigger->deactivate();
			btnFontSmaller->activate();
		}
	}

	function on_navigate(sender, udata){
		this = sender->window();
		if(sender == btnBackward && !_navigation.empty() )
		{
			view_html->topline(_navigation.top());
			_navigation.pop();
		}
		else if(sender == btnTop)
		{
			view_html->topline(0);
		}
	}
    
	function on_tab_change(sender, udata){
		this = sender->window();
		if(tabs->value() == tabView){
			if(help_text->changed2()){
				refresh_html();
				help_text->clear_changed2();
				btnSavehelp->activate();
			}
		}
	}
};
