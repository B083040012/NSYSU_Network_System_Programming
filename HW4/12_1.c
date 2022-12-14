/*
 * list the process ID and command name that given user is running
 * usage:
 *      ./12_1 <user>
 */

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
    uid_t uid;
    struct passwd *user;

    DIR *proc_dir;
    struct dirent *status_dir;
    int status_fd;

    // uid = geteuid();

    if(argc < 2){
        perror("empty argument");
        exit(1);
    }
    if((user = getpwnam(argv[1])) == NULL){
        perror("getpwnam");
        exit(1);
    }
    uid = user->pw_uid;

    if((proc_dir = opendir("/proc")) == NULL){
        perror("opendir error");
        exit(1);
    }

    while((status_dir = readdir(proc_dir)) != NULL){
        char status_dir_name[BUFFER_SIZE] = {NULL}, name[BUFFER_SIZE] = {NULL}, status_content[BUFFER_SIZE];
        int name_flag = 0, pid_flag = 0, user_flag = 0;
        pid_t pid;
        // if(strcmp(status_dir->d_name, "self") == 0) continue;
        // if(strcmp(status_dir->d_name, "thread-self") == 0) continue;
        snprintf(status_dir_name, BUFFER_SIZE, "/proc/%s/status", status_dir->d_name);
        // if((status_fd = open(status_dir_name, O_RDONLY)) == -1){
        //     if(errno == ENOENT || errno == ENOTDIR) continue;
        //     perror("open status file");
        //     exit(1);
        // }

        // if(read(status_fd, status_content, BUFFER_SIZE) == -1){
        //     perror("read status_file");
        //     exit(1);
        // }

        FILE *status_file;

        if((status_file = fopen(status_dir_name, "r")) == NULL){
            // perror("fopen");
            // exit(1);
            continue;
        }

        // if(status_content == NULL) continue;

        // fprintf(stderr, "before strtok: %s\n", status_dir->d_name);
        // fprintf(stderr, "content:\n%s\n", status_content);

        // char *line = strtok(status_content, "\n");
        // char tmp[BUFFER_SIZE];
        // strcpy(tmp, line);
        // printf("%s\n", line);

        // fprintf(stderr, "after strtok: %s\n", line);
        char line[20] = {NULL};
        // fprintf(stderr, "before fgets\n");
        if(fgets(line, 15, status_file) == NULL) continue;
        // printf("%s", line);
        while(line != NULL || (name_flag == 0 && pid_flag == 0 && user_flag == 0)){
            // printf("line: %s\n", line);
            char buf[BUFFER_SIZE] = {NULL};
            // fprintf(stderr, "before: %s\n", &line[0]);
            if(strncmp(line, "Name", 4) == 0){
                // fprintf(stderr, "name\n");
                strncpy(buf, line + 6, BUFFER_SIZE);
                strcpy(name, buf);
                name_flag = 1;
                // fprintf(stderr, "get name\n");
                // printf("name = %s\n", name);
            }
            else if(strncmp(line, "Pid", 3) == 0){
                strncpy(buf, line + 5, BUFFER_SIZE);
                pid = atoi(buf);
                pid_flag = 1;
                // fprintf(stderr, "get pid\n");
                // printf("pid = %d\n", pid);
            }
            else if(strncmp(line, "Uid", 3) == 0){
                char uid_line[BUFFER_SIZE], uid_num[BUFFER_SIZE];
                snprintf(uid_num, BUFFER_SIZE, "%d", (long)uid);
                strncpy(uid_line, line + 5, 4);
                // printf("uid_line: %s\n", uid_line);
                // printf("uid_num: %s\n", uid_num);
                if(strncmp(uid_line, uid_num, strlen(uid_num)) == 0){
                    user_flag = 1;
                    // fprintf(stderr, "get uid\n");
                }
                else{
                    break;
                }
            }
            // line = strtok(NULL, "\n");
            if(fgets(line, 20, status_file) == NULL) break;
        }

        // fprintf(stderr, "%s\n", status_dir->d_name);
        // fprintf(stderr, "done\n");

        if((name_flag == 0 || pid_flag == 0) && user_flag == 1){
            perror("process info empty");
            exit(1);
        }
        else if(user_flag == 0){
            continue;
        }
        else if(user_flag == 1){
            char *output[BUFFER_SIZE];
            snprintf(output, BUFFER_SIZE, "pid: %d, command: %s\n", pid, name);
            if(write(STDOUT_FILENO, output, strlen(output)) == -1){
                perror("write");
                exit(1);
            }
        }

        if(close(status_fd) == -1){
            perror("close");
            exit(1);
        }
    }

    if(closedir(proc_dir) == -1){
        perror("closedir for proc_dir");
        exit(1);
    }

    return 0;
}