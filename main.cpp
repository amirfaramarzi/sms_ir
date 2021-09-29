#include <iostream>
#include <sw/redis++/redis++.h>
#include <nlohmann/json.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <fstream>
#include <ctime>


using namespace std;
using namespace sw::redis;
using json = nlohmann::json;

void advanced(const json);
json getToken();
json sendFastSms(const string , const json);
void log(const string);

int main()
{
    auto redis = Redis("tcp://127.0.0.1:6379");

// Create a Subscriber.
    auto sub = redis.subscriber();

// Set callback functions.
    sub.on_message([](std::string channel, std::string msg) {
        auto data = json::parse(msg);
        auto type = data["type"];
        if (type == "advanced")
            advanced(data);


    });

// Subscribe to channels and patterns.
    sub.subscribe("sendSms");


// Consume messages in a loop.
    while (true) {
        try {
            sub.consume();
        } catch (const Error &err) {
            // Handle exceptions.
        }
    }
}

void advanced(const json data)
{
    json token = getToken();
    json send = sendFastSms(token["TokenKey"] , data);
}

json getToken()
{
    ostringstream result;
    curlpp::Cleanup cleaner;
    curlpp::Easy request;
    string url = "https://RestfulSms.com/api/Token";
    request.setOpt(new curlpp::options::Url(url));

    std::list<std::string> header;
    header.push_back("Content-Type: application/json");
    header.push_back("Accept: application/json");

    request.setOpt(new curlpp::options::HttpHeader(header));
    json dataJson;
    dataJson["UserApiKey"] = "**********"; // your UserApiKey
    dataJson["SecretKey"] = "**********"; // your SecretKey
    string data = dataJson.dump();
    request.setOpt(new curlpp::options::PostFields(data));
    request.setOpt(new curlpp::options::PostFieldSize(data.length()));
    request.setOpt(new curlpp::options::WriteStream(&result));

    request.perform();

    string res = result.str();
    json response = json::parse(res);
    if (response["IsSuccessful"] == false){
        log("url : "+url+" error : "+res);

        return getToken();
    }
    return response;
}

json sendFastSms(const string token , const json data)
{
    json dataJson;
    dataJson["Mobile"] = data["phone"];
    dataJson["TemplateId"] = data["templateId"];
    for (int i = 0; i < data["parameters"].size(); i++) {
        dataJson["ParameterArray"] = json::array({json::object({{"Parameter" , data["parameters"][i]["key"]} , {"ParameterValue" , data["parameters"][i]["value"]}})});
    }

    ostringstream result;
    curlpp::Cleanup cleaner;
    curlpp::Easy request;
    string url = "https://RestfulSms.com/api/UltraFastSend";
    request.setOpt(new curlpp::options::Url(url));

    std::list<std::string> header;
    header.push_back("Content-Type: application/json");
    header.push_back("Accept: application/json");
    header.push_back("x-sms-ir-secure-token: " + token);

    request.setOpt(new curlpp::options::HttpHeader(header));

    string dataString = dataJson.dump();
    request.setOpt(new curlpp::options::PostFields(dataString));
    request.setOpt(new curlpp::options::PostFieldSize(dataString.length()));
    request.setOpt(new curlpp::options::WriteStream(&result));

    request.perform();

    string res = result.str();
    json response = json::parse(res);
    if (response["IsSuccessful"] == false){
        log("url : "+url+" error : "+res);

        return sendFastSms(token , data);
    }
    return response;
}

void log(const string log)
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
    string str(buffer);


    string text(log+" time: "+str+"\n");
    string filename("/tmp/log.txt");
    fstream file;

    file.open(filename, std::ios_base::app);
    if (file.is_open())
        file.write(text.data(), text.size());
}