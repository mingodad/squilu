//print(sqFreetype.loadlib("/usr/lib/x86_64-linux-gnu/libfreetype.so"));
auto ft_library = new sqFreetype();
//print(ft_library);

auto function getFontGlyps(ft_library, font_fname, isMemory=false, max_glyphs=256)
{
	auto face = new sqFreetypeFace(ft_library, font_fname, 0, isMemory);
	//print(face);
	auto face_num_glyps = face.get_num_glyphs();
	if(face_num_glyps > max_glyphs) face_num_glyps = max_glyphs;
	//auto font_size = 64*12;
	auto font_size = 32*12;
	face.set_char_size(font_size, font_size);
	auto glyphs = array(face_num_glyps);
	for(auto i=0; i < face_num_glyps; ++i)
	{
		glyphs[i] = face.load_char(i);
	}
	return [face, glyphs];
}

auto function crossMatch(font1_glyphs, font2_glyphs)
{
	auto face1 = font1_glyphs[0];
	auto glyphs1 = font1_glyphs[1];

	auto face2 = font2_glyphs[0];
	auto glyphs2 = font2_glyphs[1];

	auto first_char = face2.first_char();
	print(glyphs1.len(), glyphs2.len(), first_char[0], first_char[1], face2.postscript_name());

	for(auto n0=0, len0=glyphs1.len(); n0 < len0; ++n0)
	{
		auto glyph1 = glyphs1[n0];
		for(auto n2=0, len2=glyphs2.len(); n2 < len2; ++n2)
		{
			auto glyph2 = glyphs2[n2];
			if(glyph1 == glyph2)
			{
				auto glyph_index = face2.char_index(n2);
				print("Glyphs are equal", n0, n2, face2.glyph_name(glyph_index));
				break;
			}
		}	
	}
}

auto font1_fname = "../tmp/CLFGJN_Arial_Bold.font";
//auto font1_data = readfile(font1_fname);
//auto font1_glyphs = getFontGlyps(ft_library, font1_data, true);

//auto font1_glyphs = getFontGlyps(ft_library, font1_fname);

auto font_ref_bold_glyphs = getFontGlyps(ft_library, "../tmp/Arial_Bold.ttf");
auto font_ref_glyphs = getFontGlyps(ft_library, "../tmp/Arial.ttf");
//crossMatch(font1_glyphs, font2_glyphs);

foreach(fn in ["CLFGJN_Arial_Bold.font", "CLFGKO_Arial.font", 
	"CLFGON_Helvetica.font", "CLFGPO_Helvetica_Bold.font",
	"CLFHEN_Helvetica_Bold.font", "CLFHEO_Helvetica.font"])
{
	print("\nShowing font ", fn);
	auto font_glyphs = getFontGlyps(ft_library, "../tmp/" + fn);
	crossMatch(font_glyphs, (fn.indexOf("Bold") > 0) ? font_ref_bold_glyphs : font_ref_glyphs);
}


/*
for(auto n2=0, len2=glyphs2.len(); n2 < len2; ++n2)
{
	auto glyph_index = face2.char_index(n2);
	print("Glyphs name", n2, glyph_index, face2.glyph_name(glyph_index));
}
*/
