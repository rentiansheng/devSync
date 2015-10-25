#include "config.h"

char 
is_char(int c)
{
	if( ('Z' >= c && 'A' <= c) || ('z' >= c && 'a' <= c))
		return 1;
	return 0;
}

/*
*读取配置文件的信息
*@paramter
*	path:配置文件存放的信息
*	globle_conf: 读取的配置文件内容存放的位置
*@result:
*/
int 
config_init(char *path, http_conf *g)
{
	char line[1024];
	int row;
	int err;

	FILE * f = fopen(path, "r");

	if(NULL == f) return 1;//文件不存在
	
	while(!read_line(f, line, &row)){
		if(strlen(line) == 0 ) continue;
		if((err = parse_line(f, line, &row, g))) return err;
	}

	return 0;
	
}





int 
parse_line(FILE *f, char *line,int  *row, http_conf *g) 
{
	char *split;
	char *name;
	int err;
	
	name = split = strchr(line, '=');
	if(split == NULL) return 2;//配置文件错误
	
	while(!is_char(*name)) name++;


	if(strncmp(line, "port", 4) == 0){
		split ++;

		g->port = atoi(split);
		if(g->port <= 0 || g->port > 65535)return 3;//port不正确
	}
	else if(strncmp(line, "web", 3) == 0) {
		if((err = set_web(f, g, row))) return err;
		g->web_count ++;
	}
	else if(strncmp(line, "mimetype", 8) == 0){
		if((err = set_mimetype(f, g, row))) return err;;
	}
	else {
		return 4;//不能识别的配置节点
	}

	return 0;
}


int 
set_mimetype(FILE * f, http_conf *g, int *row)
{
	char line[1024];
	char *name, *split, *end;
	char *value;
	int len;
	struct key *k;


	while(!read_line(f, line, row)){
		if(strlen(line) == 0) continue;
		name = line;
		split = strchr(name, ':');
		if(split == NULL) {
			if(strchr(name, ')') == NULL) return 2; else break;
		}
		end = split - 1;
		while(*end == ' ' &&  end >= name) end--;
		if(end < name) return 5;//配置节点的名字不能为空
		len = end - name  + 2;
		value = (char *) malloc(sizeof(char)*(len));
		if(k == NULL) return MEMERROR;//memory error
		strncpy(value, name,  len);
		value[len-1] = 0;

		name = split+1;
		while(*name == ' ') name++;
		end = &split[strlen(split) - 1];
		while(*end == ' ' && end >= name)end --;
		if(end < name) return 5;//配置节点的名字不能为空
		len = end - name + 2;
		
		k = (struct key *)malloc(sizeof(struct key));
		if(k == NULL) return MEMERROR;// memory error;
		k->name = value;

		k->value = (char *)malloc(sizeof(char) * len);
		if(k->value == NULL) return MEMERROR;
		strncpy(k->value, name, len);
		k->value[len -1] = 0;

		k->next = g->mimetype;
		g->mimetype = k;

	}
	return 0;
}


int 
set_web(FILE *f, http_conf *g, int *row)
{
	char line[1024];
	char *name, *split, *end;
	char *value,  *item;
	int len;
	struct web_conf *web;
	
	web = (struct web_conf *)malloc(sizeof(web_conf));
	if(web == NULL) return MEMERROR;

	while(!read_line(f, line, row)){
		if(strlen(line) == 0) continue;
		name = line;
		split = strchr(name, ':');
		
		if(split == NULL)
		{
			 if(strchr(name, ')') == NULL)return 2; else break;
		}
		end = split - 1;
		while(*end == ' ' && end >= name) end--;
		if(end < name) return 5;//配置节点的名字不能为空
		len = end - name  + 2;
		value = (char *) malloc(sizeof(char)*(len));
		if(value == NULL) return MEMERROR;//memory error
		strncpy(value, name, len);
		value[len] = 0;
		
		name = split+1;
		while(*name == ' ') name++;
		end = &split[strlen(split) - 1];
		while(*end-- == ' ' && end >= name)end --;
		if(end < name) return 5;//配置节点的名字不能为空
		len = end - name + 2;
		item = (char *) malloc (sizeof(char)*len);
		if(item == NULL) return MEMERROR;//memory error
		strncpy(item, name, len);
		item[len] = 0;
		if(strncmp("root", value, 4) == 0) {
				web->root = item;
		}
		else if(strncmp( "indexfiles", value, 10) == 0) {
				web->index_count = 0;
				web->index_file = item;
				
				if(strlen(item) != 0) web->index_count = 1;
				while((item = strchr(item, ',')) != NULL) {
					*item = 0;
					item++;
					web->index_count++;
				}

		}
		else if(strncmp("404_page", value, 8) == 0) {
				web->err404 = item;
		}
		else {
				free(value);
				return 4;//不能识别的配置节点
		}
		free(value);

	}
	
	web->next = g->web;
	g->web = web;

	return 0;	
}


int
read_line(FILE *f, char *line, int *row)
{
	int zs = 0;
	int count;
	int c;
	
	row++;
	count = 0;
	while((c = fgetc(f)) !=EOF){
		if(zs == 1 && c != '\n') continue;
		if('#' == c){zs = 1;continue;}
		if(c == '\t' || c == '"') continue;
		if(c == '\n'){
			zs = 0;
			line[count] = 0;
			return 0;
		}
		if(c == ' ' && zs == 0)continue;
		line[count++] = c;
		zs = 2;
	}
	
	return 1;//read end

}

