/*
 * Copyright (C) Reage
 * BLOG: http://www.ireage.com
 */

#include "http_header.h"

static char *skip_space(char *start, char *end)
{
	if (start == NULL)
		return NULL;
	while (*start == ' ')
	{
		if (start >= end)
			return NULL;
		start++;
	}

	return start;
}

static char *find_line(char *start, char *end)
{
	while (*start != '\n')
	{
		if (start >= end)
			return NULL;
		start++;
	}

	return start;
}

static int parse_http_request_row(pool_t *p, request *in, char *start, char *end)
{

	if (strncasecmp(start, "put", 3) == 0)
	{
		in->http_method = _PUT;
		start += 3;
	}
	else if (strncasecmp(start, "get", 3) == 0)
	{
		in->http_method = _GET;
		start += 3;
	}
	else if (strncasecmp(start, "post", 4) == 0)
	{
		in->http_method = _POST;
		start += 4;
	}
	else if (strncasecmp(start, "service", 7) == 0)
	{
		in->http_method = _SERVICE;
		start += 7;
	}
	else if (strncasecmp(start, "cgi", 3) == 0)
	{
		in->http_method = _CGI;
		start += 3;
	}
	else if (strncasecmp(start, "del", 3) == 0)
	{
		in->http_method = _DEL;
		start += 3;
	}
	else
	{
		in->http_method = _NONE;
	}

	start = skip_space(start, end);
	in->uri = string_init(p);
	in->uri->ptr = start;
	start = find_line(start, end);
	char *ptr = in->uri->ptr;
	while (ptr < start && *ptr != ' ' && *ptr != '?')
	{
		ptr++;
	}

	in->uri->len = ptr - in->uri->ptr;

	return 0;
}

static int parse_content_length_compatible_old(request *in, char *start)
{
	if (start == NULL)
	{
		in->content_length = 0;
		return 0;
	}
	in->content_length = atoi(start);

	return 0;
}

static int parse_http_header_messge(pool_t *p, request *in, char *start, char *end)
{

	string *line = string_init(p);
	string *key = string_init(p);
	string *value = string_init(p);

	while (start != NULL && start < end)
	{
		if (*start == '\n')
		{
			start++;
			continue;
		}
		string_get_line(start, end, line);
		if (line->len == 0 || line->ptr == NULL)
		{
			break;
		}
		string_get_word_with_split(line, key, ':');
		if (key->len == 0 || key->ptr == NULL)
		{
			start = line->ptr + line->len;
			continue;
		}
		start += key->len + 1;
		string_get_line(start, end, value);
		if (value->len == 0 || key->ptr == NULL)
		{
			start = line->ptr + line->len;
			continue;
		}
		if (strncasecmp("content-length", key->ptr, key->len) == 0)
		{
			in->content_length = atoi(value->ptr);
		}
		else if (strncasecmp("execute-file", key->ptr, key->len) == 0)
		{
			in->execute_file = string_init(p);
			in->execute_file->ptr = value->ptr;
			in->execute_file->len = value->len;
		}
		else if (strncasecmp("time", key->ptr, key->len) == 0)
		{
			in->ts = atoi(value->ptr);
		}

		start = line->ptr + line->len;
	}

	return 0;
}

static void test_print_header(request *in)
{
	printf("uri=%s  len =%d\n", in->uri->ptr, in->uri->len);

	printf("content_length %d \n", in->content_length);

	if (in->execute_file != NULL)
	{
		printf("execute file %s %d", in->execute_file->ptr, in->execute_file->len);
	}

	printf("%s", in->header->ptr);
}

/**
 *0 读取结束，其他没有结束
 *
 */
int read_header(http_connect_t *con)
{
	pool_t *p;
	buffer *header;
	char *ptr;
	char c;

	p = (pool_t *)con->p;

	if (con->in->header == NULL)
	{
		con->in->header = buffer_create_size(p, MAX_HEADER_SIZE);
	}
	header = con->in->header;

	while (read(con->fd, &c, 1) == 1)
	{
		if (header->used == MAX_HEADER_SIZE)
		{
			con->next_handle = send_put_header_err;
			return NEXT;
		}
		buffer_append_char(header, c, p);
		if (c == '\n' && header->used >= 2)
		{
			ptr = header->ptr + header->used - 2;
			if (strncasecmp(ptr, "\n\n", 2) == 0 || strncasecmp(ptr - 2, "\r\n\r\n", 4) == 0)
			{
				con->next_handle = parse_http_handler;
				return NEXT;
			}
		}
	}

	return CONTINUE;
}

int parse_http_handler(http_connect_t *con)
{
	// todo parse_header error not handle
	parse_header(con);

	ds_log(con, "  [ACCEPT] ", LOG_LEVEL_DEFAULT);

	if (con->in->http_method == _PUT)
	{
		//open_write_file(con);
		con->next_handle = open_write_file;
	}
	else if (con->in->http_method == _GET)
	{
		con->next_handle = send_execute;
	}
	else if (con->in->http_method == _DEL)
	{
		con->next_handle = delete_resource;
	}
	else
	{
		con->next_handle = NULL; //最好输出不支持的信息
		return DONE;
	}

	return NEXT;
}

void parse_header(http_connect_t *con)
{
	request *in;
	buffer *header;
	pool_t *p;
	char *start, *end;

	p = (pool_t *)con->p;
	in = con->in;
	header = in->header;
	start = (char *)(header->ptr);
	end = (char *)header->ptr + (header->used - 1);

	parse_http_request_row(p, in, start, end); //请求行

	start = find_line(start, end);
	if (start != NULL)
	{
		start++;
	}
	parse_content_length_compatible_old(in, start);

	parse_http_header_messge(p, in, start, end);

	//test_print_header(in);
	return;
}
