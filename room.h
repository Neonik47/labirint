void put_file(char *filename, int newsockfd){
	if(strlen(filename)){
		char *line=(char*)malloc(sizeof(char)*20);
		sprintf(line,"files/%s",filename);
		FILE *pFile = fopen(line, "r");
		if(pFile == NULL)
			write(newsockfd, err, strlen(err));
		else{
			while (1) {
				bzero(line,20);
				fgets(line,2,pFile);

				if(feof(pFile))
					break;
			}
			fclose(pFile);
		}
		write(newsockfd, to_root, strlen(to_root));
	}else{
		DIR * dir;
		struct dirent * entry;
		char s[1000];
		if ((dir = opendir("files")) == NULL) {
			perror("opendir");
			write(newsockfd, err, strlen(err));
			close(newsockfd);
		}
		while ((entry = readdir(dir))) {
			if (entry -> d_type == 8){
				bzero(s,1000);
				sprintf(s, "<a href = '%s'>%s</a><br>",entry -> d_name, entry -> d_name);
				write(newsockfd,s, strlen(s));
			}
		}
		
	}
}
