function getFilesRecursing(base_dir, files)
{
	foreach(fname in sqfs.dir(base_dir)){
		if(fname == "." || fname == "..")
		{
			continue;
		}
		var file_full_path = base_dir + "/" + fname;
		if(sqfs.attributes(file_full_path, "mode") == "directory")
		{
			getFilesRecursing(file_full_path, files);
		} else {
			files.append(file_full_path);
		}
	}
}

var files = [];
getFilesRecursing(".", files);

for(var i=0, len=files.len(); i < len; ++i)
{
	print(files[i]);
}
