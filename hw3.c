


int main(){
    char input[80];
    while(1){
        printf(">");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error reading input");
            exit(1);
        }
        char input[100];
        scanf("%s", input)
        char * tokens = strtok(input, " ");
  
        if(strcmp(input, "read") == 0){

  
        }
        else if(strcmp(input, "write") == 0){

        }
        else if(strcmp(input, "showmain") == 0){

        }
        else{
            printf("Invalid command\n");
        }

    }
}
  