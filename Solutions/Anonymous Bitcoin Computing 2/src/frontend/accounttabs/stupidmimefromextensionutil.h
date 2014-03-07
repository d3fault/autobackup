#ifndef STUPIDMIMEFROMEXTENSIONUTIL_H
#define STUPIDMIMEFROMEXTENSIONUTIL_H

#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

using namespace std;

class StupidMimeFromExtensionUtil
{
public:
    //pair because "svg+xml" wouldn't make a good filename extension (i'm completely surrounded by morons)...
    static pair<string, string> guessExtensionAndMimeType(string filename) //TODOoptional: delete this shit and use image header magic ONLY. can probably also not store the extension in the db as well and just re-analyze header.. but bleh
    {
        //std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower); //ugly, Qt5Life (had: <guess>)

        //boost > std/stl, but boost < qt. already depending on boost though, so...
        boost::algorithm::to_lower(filename);

        pair<string,string> guessedExtensionAndMime(".jpg", "jpeg");
        if(boost::algorithm::ends_with(filename, ".jpg") || boost::algorithm::ends_with(filename, ".jpeg"))
        {
            return guessedExtensionAndMime;
        }
        if(boost::algorithm::ends_with(filename, ".bmp")) //TODOoptimization: maybe security risk, and definitely adds dependencies, but would save bandwidth, to convert this to png
        {
            guessedExtensionAndMime.first = ".bmp";
            guessedExtensionAndMime.second = "bmp";
            return guessedExtensionAndMime;
        }
        return guessedExtensionAndMime;
#if 0 //used to allow below kinds, but now don't
        if(boost::algorithm::ends_with(filename, ".png"))
        {
            guessedExtensionAndMime.first = ".png";
            guessedExtensionAndMime.second = "png";
            return guessedExtensionAndMime;
        }
        if(boost::algorithm::ends_with(filename, ".gif"))
        {
            guessedExtensionAndMime.first = ".gif";
            guessedExtensionAndMime.second = "gif";
            return guessedExtensionAndMime;
        }
        if(boost::algorithm::ends_with(filename, ".svg"))
        {
            guessedExtensionAndMime.first = ".svg";
            guessedExtensionAndMime.second = "svg+xml";
            return guessedExtensionAndMime;
        }
        if(boost::algorithm::ends_with(filename, ".webp"))
        {
            guessedExtensionAndMime.first = ".webp";
            guessedExtensionAndMime.second = "webp";
            return guessedExtensionAndMime;
        }
        return guessedExtensionAndMime;
#endif
    }
};

#endif // STUPIDMIMEFROMEXTENSIONUTIL_H
