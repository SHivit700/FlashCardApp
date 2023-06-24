#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"

struct memory {
  char *response;
  size_t size;
};
 
static size_t cb(void *data, size_t size, size_t nmemb, void *clientp)
{
  size_t realsize = size * nmemb;
  struct memory *mem = (struct memory *)clientp;
 
  char *ptr = realloc(mem->response, mem->size + realsize + 1);
  if(ptr == NULL)
    return 0;  /* out of memory! */
 
  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;
  mem->response[mem->size] = 0;
 
  return realsize;
}

char* add20(char* inp) {
  char *out = (char*)calloc(100, sizeof(char));

  for(int i = 0; i < strlen(inp); i++) {
    if(inp[i] == ' ') {
      strcat(out, "%20");
    } else {
      strncat(out, &inp[i], 1);
    }
  }

  return out;
}

static char* makeRequest(char *url) {
  struct memory chunk = {0};
  CURLcode ret;
  CURL *hnd;
  
  curl_global_init(CURL_GLOBAL_DEFAULT);
  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(hnd, CURLOPT_URL, url);
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.84.0");
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, cb);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&chunk);

  ret = curl_easy_perform(hnd);

  if(ret != 0) {
    return "";
  }

  curl_easy_cleanup(hnd);
  hnd = NULL;
  curl_global_cleanup();
  return chunk.response;
}

char* getTarget(char* input) {
  char* url = (char *)calloc(strlen("https://en.wikipedia.org/w/api.php\?action=query&format=json&list=search&utf8=1&formatversion=2&srsearch=") + strlen(input), sizeof(char));
  strcat(url, "https://en.wikipedia.org/w/api.php\?action=query&format=json&list=search&utf8=1&formatversion=2&srsearch=");
  strcat(url, input);

  free(input);

  char* response = makeRequest(url);

  cJSON *json = cJSON_Parse(response);

  if (json == NULL) {
      return NULL;
  }


  cJSON *query = cJSON_GetObjectItemCaseSensitive(json, "query");
  cJSON *searches = cJSON_GetObjectItemCaseSensitive(query, "search");
  cJSON *search = NULL;
  char *target = NULL;

  cJSON_ArrayForEach(search, searches) {
    cJSON *title = cJSON_GetObjectItemCaseSensitive(search, "title");

    if(cJSON_IsString(title) && title->valuestring != NULL) {
      // printf("%s\n", cJSON_Print(title));
      // printf("%s\n", title->valuestring);
      target = title->valuestring;
      goto end;
    }
  }
end:
  // printf("target: %s\n", target);
  return target;
}

char* getBody(char *input) {
  char* url = (char *)calloc(strlen("https://en.wikipedia.org/w/api.php?action=query&prop=extracts&exintro&explaintext&format=json&titles=") + strlen(input), sizeof(char));
  strcat(url, "https://en.wikipedia.org/w/api.php?action=query&prop=extracts&exintro&explaintext&format=json&titles=");
  strcat(url, input);

  free(input);

  // printf("URL: %s\n", url);
  char* response = makeRequest(url);
  // printf("RESPONSE: %s\n", response);

  cJSON *json = cJSON_Parse(response);

  if(json == NULL) {
    return NULL;
  }


  // printf("body JSON result: %s\n", cJSON_Print(json));
  cJSON *query = cJSON_GetObjectItemCaseSensitive(json, "query");
  cJSON *pages = cJSON_GetObjectItemCaseSensitive(query, "pages");
  cJSON *extract = cJSON_GetObjectItemCaseSensitive(pages->child, "extract");
  
  char *target = NULL;
  if(cJSON_IsString(extract) && extract->valuestring != NULL) {
    target = extract->valuestring;
  }

  return target;
}

// int main(int argc, char *argv[])
// {
//   char* arg = add20(argv[1]);
//   printf("Arg: %s\n", arg);
//   char* target = getTarget(arg);
//   printf("Target title: %s\n", target);
//   printf("Target extract: %s\n", getBody(add20(target)));
//   //printf("Target extract: %s\n", getBody(add20("Hello")));
//   return 0;
// }
