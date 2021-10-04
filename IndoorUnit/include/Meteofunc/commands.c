//extern SDFS SD;
extern WiFiClient RemoteClient;

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    RemoteClient.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        RemoteClient.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        RemoteClient.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            RemoteClient.print("  DIR : ");
            RemoteClient.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            RemoteClient.print("  FILE: ");
            RemoteClient.print(file.name());
            RemoteClient.print("  SIZE: ");
            RemoteClient.println(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path){
    //RemoteClient.printf("Reading file: %s\n", path);

    File file = fs.open(path);
   
    if(!file){
        RemoteClient.print("0000");
        RemoteClient.println("Failed to open file for reading");
        return;
    }
    long int filelenth = file.size();
    for (int i = 4; i > 0; i--)
    {
        RemoteClient.write((uint8_t)(filelenth >> ((i-1)*8)));
    }
    //RemoteClient.println("Read from file: ");
    while(file.available()){
        RemoteClient.write(file.read());
    }
    file.close();
}

bool compare(char * str1, const char * str2, uint8_t n)
{
	bool equals = true;

	for (uint8_t index = 0; index < n; index++)
	{
		if (str1[index] != str2[index])
		{
			equals = false;
			break;
		}
	}
   return equals;
}