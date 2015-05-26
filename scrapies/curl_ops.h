#ifndef ba57c2d4bd1642939df1a29e681b1658
#define ba57c2d4bd1642939df1a29e681b1658
#include <curl/curl.h>
#include <sstream>

namespace scrapies_curl
{
    struct FtpFile {
        const char *filename;
        FILE *stream;
    };

    size_t write_to_file(void *buffer, size_t size, size_t nmemb, void *stream)
    {
        struct FtpFile *out=(struct FtpFile *)stream;
        if(out && !out->stream) {
            /* open file for writing */
            out->stream=fopen(out->filename, "wb");
            if(!out->stream)
                return -1; /* failure, can't open file to write */
        }
        return fwrite(buffer, size, nmemb, out->stream);
    }
    
    size_t write_to_string(char * buffer, size_t size, size_t nmemb, void *stream)
    {
        size_t count = size * nmemb;
        ((std::ostringstream *)stream)->write(buffer, nmemb);
        return count;
    }

    //TODO(brian): is write_ftp thread safe?
    std::string get_file(const char * t_output, const char * t_path)
    {
        CURL *curl;

        struct FtpFile file = {t_output, NULL };

        curl = curl_easy_init();
        if(curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, t_path);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_file);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

            //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            CURLcode res = curl_easy_perform(curl);

            /* always cleanup */
            curl_easy_cleanup(curl);

            if(CURLE_OK != res)
            {
                /* we failed */
                fprintf(stderr, "curl told us %d\n", res);
                return "";
            }
        }

        if(file.stream)
            fclose(file.stream); /* close the local file */
        
        return file.filename;
    }
    
    std::stringstream get_string(const char * t_path)
    {
        CURL *curl;

        std::stringstream stream;

        curl = curl_easy_init();
        if(curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, t_path);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);

            //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            CURLcode res = curl_easy_perform(curl);

            /* always cleanup */
            curl_easy_cleanup(curl);

            if(CURLE_OK != res)
            {
                /* we failed */
                fprintf(stderr, "curl told us %d\n", res);
                return "";
            }
        }
        printf("%s", stream.str().c_str());
        return std::move(stream);
    }
}
#endif