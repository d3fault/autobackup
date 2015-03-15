#include "recursivecustomdetachedgpgsignatures.h"

#define GPG_END_SIG_DELIMITER "-----END PGP SIGNATURE-----"

RecursiveCustomDetachedSignatures::RecursiveCustomDetachedSignatures(QObject *parent)
    : QObject(parent)
{ }
bool RecursiveCustomDetachedSignatures::readPathAndSignature(QTextStream &customDetachedGpgSignaturesTextStream, QString *out_FilePathToVerify, QString *out_CurrentFileSignature)
{
    *out_FilePathToVerify = customDetachedGpgSignaturesTextStream.readLine();
    QString lastReadLine;
    do
    {
        if(customDetachedGpgSignaturesTextStream.atEnd())
        {
            emit e("malformed custom detached gpg signature file");
            return false;
        }
        lastReadLine = customDetachedGpgSignaturesTextStream.readLine(); //TO DOnereq(/dumb): getting a consistent segfault (sigsegv) right here and have no clue why, I'm wondering if it's a Qt bug. different dirs/sig files crash at the same spot every time. tried all sorts of combinations/rewrites. rolled back to before latest addition, still there. shit rolled back to first commits of this app, still there. WTF!?!? --- maybe 30 minutes later: omg figured it out, my QFile was going out of scope (deep down I knew it was my fault). How the fuck did I see like 10+ files have their sigs verified then? That's even weirder, though I don't care to try to understand why...
        out_CurrentFileSignature->append(lastReadLine + "\n"); //TODOoptional: might be better to not use readLine up above and to just read until the string is seen (retain whatever newline form the output of gpg gave us to begin with)
    }
    while(lastReadLine != GPG_END_SIG_DELIMITER);
    return true;
}
