local xml = TinyXml2.XMLDocument();
local element = xml->InsertEndChild( xml->NewDeclaration() );
local text = xml->InsertEndChild( xml.NewText("name") );
text.Value("Domingo");
print(text, text instanceof TinyXml2.XMLText);
local comment = xml->InsertEndChild( xml.NewComment("Notes") );
comment.Value("Hello World !");
print(comment, type(comment));
xml.SaveFile("test.xml");
print(xml.tostring());
xml.LoadFile("test.xml");
print(xml.tostring());

local doc = new TinyXml2.XMLDocument();
element = doc->InsertEndChild( doc->NewDeclaration() );
element = doc->InsertEndChild( doc->NewElement( "name" ) );
element->InsertEndChild( doc->NewText( "Domingo" ) );
element = doc->InsertEndChild( doc->NewElement( "parent" ) );
text = element->InsertEndChild( doc->NewText( "Tais Brasil]" ) );
text.CData(true);

doc.SaveFile("doc.xml");
print(doc.tostring());

local printer = TinyXml2.XMLPrinter ();
printer.PushHeader(true, true);
printer.OpenElement( "foo" );
printer.PushAttribute( "foo", "bar" );
printer.CloseElement();
print(printer.tostring());


