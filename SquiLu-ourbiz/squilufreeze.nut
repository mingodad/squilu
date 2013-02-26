//////////////////////////////////////////////////////////////////////////////-
// Name:        squilufreeze.nut
// Purpose:     Append a squilu script to the squiluFreeze executable to run
// Author:      John Labenski
// Created:     2005
// Copyright:   (c) 2005 John Labenski
// Licence:     wxWidgets licence
// Adapted to SquiLu by Domingo Alvarez Duarte
//////////////////////////////////////////////////////////////////////////////-
// This program loads a squilu script and appends it to the end of the given
//   squiluFreeze C++ program to run it.
//
// See "Usage()" function for usage or just run this with no parameters.
//////////////////////////////////////////////////////////////////////////////-

// simple test to see if the file exits or not
function FileExists(filename){
	try {
		local fd = file(filename, "r");
		fd.close();
		return true;
	}
	catch(e){
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////-

// Print the Usage to the console
function Usage(){
  print("squilufreeze.nut appends a squilu program to a precompiled application.");
  print("Usage : squilufreeze.nut input_squilu input_squilu_script.nut output_squiluFreezeApp");
  print("");
  print(" If using unix, don't forget to $chmod +x output_squiluFreezeApp");
}

// The main() program to run
function main(){

    if ( vargv.len() != 4) {
        Usage();
        return;
    }

    local in_squiluFreeze  = vargv[1];
    local in_squiluScript  = vargv[2];
    local out_squiluFreeze = vargv[3];

    if (!FileExists(in_squiluFreeze)) {
        printf("Input squiluFreeze program '%s' does not exist! Aborting.\n", in_squiluFreeze);
        return;
    }
    if (!FileExists(in_squiluScript)){
        printf("Input luafltk script '%s' does not exist! Aborting.\n", in_squiluScript);
        return;
    }
    if (FileExists(out_squiluFreeze)){
        printf("Output squiluFreeze program '%s' will be overwritten.\n", out_squiluFreeze);
    }

    printf("Appending '%s' to squiluFreeze program '%s' and outputting to '%s'\n",
            in_squiluScript, in_squiluFreeze, out_squiluFreeze);

    // read the whole squiluFreeze program into memory
    local in_squiluFreeze_handle = file(in_squiluFreeze, "rb");
    local in_squiluFreeze_length =in_squiluFreeze_handle.len();
    local in_squiluFreeze_data   = in_squiluFreeze_handle.read(in_squiluFreeze_length);
    in_squiluFreeze_handle.close();

    // check to see if they're trying to append another script to squiluFreeze
    if (in_squiluFreeze_data.slice(in_squiluFreeze_length-21, in_squiluFreeze_length-10) == "squiluFreeze:"){
        printf("Input squiluFreeze program '%s' already has a squilu script attached! Aborting.\n", in_squiluFreeze);
        return;
    }

    // read the whole luafltk script into memory
    local in_squiluScript_handle = file(in_squiluScript, "rb")
    local in_squiluScript_length = in_squiluScript_handle.len();
    local in_squiluScript_data   = in_squiluScript_handle.read(in_squiluScript_length);
    in_squiluScript_handle.close()
    
    local zipedScript = zlib.deflate(in_squiluScript_data);
    print("zipedScript", in_squiluScript_length, zipedScript.len());
    in_squiluScript_length = zipedScript.len();
    in_squiluScript_data = zipedScript;

    // write the output file and our tag at the end
    local out_squiluFreeze_handle = file(out_squiluFreeze, "wb+");
    out_squiluFreeze_handle.write(in_squiluFreeze_data);
    out_squiluFreeze_handle.write(in_squiluScript_data);
    // 2^32 = 4294967296, should be enough? Our data is 29 bytes from end.
    out_squiluFreeze_handle.write(format("<DAD:%010d>ooOo(^.^)oOoo", in_squiluScript_length));
    out_squiluFreeze_handle.close();
}

main();
