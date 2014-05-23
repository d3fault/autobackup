#include "d3faultpubliclicensecopyrightheaderbulkprepender.h"

#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QHashIterator>
#include <QTextStream>

#define D3faultPublicLicenseCopyrightHeaderBulkPrepender_DEFAULT_COPYRIGHT_HEADER_FILE_PATH "../../../header.prepend.dpl.v3.d3fault.launch.distribution.txt"

//mind = blown: if calling this script from hooks/post-update, do i first compile from the latest version (may or may not have been changed) that the git-push we are responding to thing'd?
D3faultPublicLicenseCopyrightHeaderBulkPrepender::D3faultPublicLicenseCopyrightHeaderBulkPrepender(QObject *parent)
    : QObject(parent)
{
    QStringList fileExtensionsWithCstyleComments;
    QStringList fileExtensionsWithSimpleHashStyleComments;
    QStringList fileExtensionsWithShellScriptHashStyleComments;
    QStringList fileExtensionsWithPhpStyleComments;
    QStringList fileExtensionsWithNoComments;
    QStringList fileExtensionsWithXmlStyleComments;

    QStringList fileExtensionsWithHeaderStyleComments;

    fileExtensionsWithCstyleComments << "c" << "cpp" << "h" << "java" << "cs"; //h would be included, but since not dpl yet is not
    fileExtensionsWithSimpleHashStyleComments << "pro" << "asm" << "s"; //for stripping it's ok to have sh here, but not for inserting! << "sh";
    fileExtensionsWithPhpStyleComments << "php" << "phps";
    fileExtensionsWithNoComments << "txt";
    fileExtensionsWithXmlStyleComments << "svg"; //TODOreq: .xml files? html files?
    fileExtensionsWithShellScriptHashStyleComments << "sh";

    fileExtensionsWithHeaderStyleComments << "h" << "hpp";

    m_FileStyleExtensionsHash.insert(NoCommentsPlaintextFileStyle, fileExtensionsWithNoComments);
    m_FileStyleExtensionsHash.insert(CFileStyle, fileExtensionsWithCstyleComments);
    m_FileStyleExtensionsHash.insert(PhpFileStyle, fileExtensionsWithPhpStyleComments);
    m_FileStyleExtensionsHash.insert(SimpleHashFileStyle, fileExtensionsWithSimpleHashStyleComments);
    m_FileStyleExtensionsHash.insert(ShellScriptHashFileStyle, fileExtensionsWithShellScriptHashStyleComments);
    m_FileStyleExtensionsHash.insert(XmlFileStyle, fileExtensionsWithXmlStyleComments);

    m_FileStyleExtensionsHash.insert(CStyleHeaderProprietaryAllRightsReservedForNow, fileExtensionsWithHeaderStyleComments);
}
bool D3faultPublicLicenseCopyrightHeaderBulkPrepender::bulkPrependD3faultPublicLicenseCopyrightHeaderPrivate(const QString &dirToPrependTo, const QString &optionalAlternateCopyrightHeaderFilePath)
{
    if(dirToPrependTo.isEmpty())
    {
        emit e("dirToPrependTo can't be empty");
        return false;
    }
    QFileInfo dirFileInfo(dirToPrependTo);
    if(!dirFileInfo.isDir())
    {
        emit e("dirToPrependTo ( " + dirToPrependTo + " ) is not a dir");
        return false;
    }

    QString copyrightHeaderFilePath = optionalAlternateCopyrightHeaderFilePath;
    if(optionalAlternateCopyrightHeaderFilePath.isEmpty())
        copyrightHeaderFilePath = D3faultPublicLicenseCopyrightHeaderBulkPrepender_DEFAULT_COPYRIGHT_HEADER_FILE_PATH;

    QFileInfo copyrightHeaderFileInfo(copyrightHeaderFilePath);
    QString copyrightHeaderCanonicalFilePath(copyrightHeaderFileInfo.canonicalFilePath());

    QDirIterator dirIterator(dirToPrependTo, (QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs | QDir::Hidden), QDirIterator::Subdirectories);
    while(dirIterator.hasNext()) //Random/OT: integrating the dynamic responsiveness with... say.. qdiriterator... sounds challenging (but possible)
    {
        dirIterator.next();
        const QFileInfo &currentFileInfo = dirIterator.fileInfo();
        if(currentFileInfo.isFile())
        {
            if(currentFileInfo.canonicalFilePath() == copyrightHeaderCanonicalFilePath) //don't prepend onto the prepend source!
                continue;

            QString suffixToLower = currentFileInfo.suffix().toLower();

            QHashIterator<CopyrightHeaderFileStyleEnum, QStringList> extensionsIterator(m_FileStyleExtensionsHash);
            while(extensionsIterator.hasNext());
            {
                extensionsIterator.next();
                if(extensionsIterator.value().contains(suffixToLower))
                {
                    if(!insertFormattedCopyrightHeaderIntoFileAtLine(whereToInsertCopyrightHeadersForFileStyle(extensionsIterator.key()), lazyFormatCopyrightHeaderForFileStyle(extensionsIterator.key()), currentFileInfo.canonicalFilePath())) //so sex
                    {
                        emit e("failed to prepend copyright header to: " + currentFileInfo.canonicalFilePath());
                        return false;
                    }
                    break;
                }
            }
        }
        else if(currentFileInfo.isDir())
        {
            emit o("copyright header insert is entering dir: " + currentFileInfo.absoluteFilePath());
        }
    }
    return true;
}
uint D3faultPublicLicenseCopyrightHeaderBulkPrepender::whereToInsertCopyrightHeadersForFileStyle(D3faultPublicLicenseCopyrightHeaderBulkPrepender::CopyrightHeaderFileStyleEnum copyrightHeaderFileStyle)
{
    /*switch(copyrightHeaderFileStyle)
    {
    case ShellScriptHashFileStyle:
    case XmlFileStyle:
        return 1;
    case NoCommentsPlaintextFileStyle:
    case CFileStyle:
    case SimpleHashFileStyle:
    case PhpFileStyle:
    case CStyleHeaderProprietaryAllRightsReservedForNow:
    default:
        return 0;
    }*/

    if(copyrightHeaderFileStyle == ShellScriptHashFileStyle || copyrightHeaderFileStyle == XmlFileStyle)
        return 1;
    return 0;
}
//memory conservative, was more fun to write than it's even worth!
QString D3faultPublicLicenseCopyrightHeaderBulkPrepender::lazyFormatCopyrightHeaderForFileStyle(D3faultPublicLicenseCopyrightHeaderBulkPrepender::CopyrightHeaderFileStyleEnum copyrightHeaderFileStyle)
{
    QString formattedCopyrightHeader = m_LazyFormattedCopyrightHeaderHash.value(copyrightHeaderFileStyle);
    if(formattedCopyrightHeader.isEmpty())
    {
        emit o("good: encountered new copyright file style based on extension. enum val (lol, i don't have the actual file extension handy at this level) = " + QString::number(copyrightHeaderFileStyle));
        formattedCopyrightHeader = formatCopyrightHeaderForFileStyle(copyrightHeaderFileStyle);
        m_LazyFormattedCopyrightHeaderHash.insert(copyrightHeaderFileStyle, formattedCopyrightHeader);
    }
    return formattedCopyrightHeader;
}
QString D3faultPublicLicenseCopyrightHeaderBulkPrepender::formatCopyrightHeaderForFileStyle(D3faultPublicLicenseCopyrightHeaderBulkPrepender::CopyrightHeaderFileStyleEnum copyrightHeaderFileStyle)
{
    if(copyrightHeaderFileStyle == CFileStyle)
    {
        QString ret = "/*\n" + m_CopyrightHeaderUnmodified + "\n*/\n\n";
        return ret;
    }
    if(copyrightHeaderFileStyle == PhpFileStyle)
    {
        QString ret = "<?php /*\n" + m_CopyrightHeaderUnmodified + "\n*/ ?>\n\n";
        return ret;
    }
    if(copyrightHeaderFileStyle == SimpleHashFileStyle || copyrightHeaderFileStyle == ShellScriptHashFileStyle) //formatting is the same, but position inserted at is not
    {
        QString ret = m_CopyrightHeaderUnmodified;
        ret.insert(0, "#");
        ret.replace('\n', "\n#"); //TODOreq: test that this works. should and might, but unsure
        return ret;
    }
    if(copyrightHeaderFileStyle == XmlFileStyle)
    {
        QString ret = m_CopyrightHeaderUnmodified;
        //replace all "--" in the copyright header, since apparently they can't be in an xml comment -_- (NOOBS)
        QString tempForComparingReplacementProgress;
        do
        {
            tempForComparingReplacementProgress = ret;
            ret.replace("--", "__");
        }
        while(ret != tempForComparingReplacementProgress);
        QString temp = ret;
        ret = "<!--\n" + temp + "\n-->\n\n";
        return ret;
    }
    if(copyrightHeaderFileStyle == CStyleHeaderProprietaryAllRightsReservedForNow)
    {
        QString ret = "/*\nThis file is part of the d3fault launch distribution\nCopyright (C) 2014 Steven Curtis Wieler II\nAll Rights Reserved\n*/\n\n";
        return ret;
    }

    if(copyrightHeaderFileStyle != NoCommentsPlaintextFileStyle)
    {
        emit e("no format for that CopyrightHeaderFileStyle ( enumVal = " + QString::number(copyrightHeaderFileStyle) + " ) found, using plaintext"); //should never get here...
    }
    QString noCommentsPlaintextFormat = m_CopyrightHeaderUnmodified + "\n\n";
    return noCommentsPlaintextFormat;
}
bool D3faultPublicLicenseCopyrightHeaderBulkPrepender::insertFormattedCopyrightHeaderIntoFileAtLine(uint lineToInsertAt, const QString &formattedCopyrightHeaderText, const QString &absoluteFilePathOfFileToInsertCopyrightHeaderInto)
{
    QFile fileToInsertInto(absoluteFilePathOfFileToInsertCopyrightHeaderInto);
    bool ret = insertFormattedCopyrightHeaderIntoIoDeviceAtLine(lineToInsertAt, formattedCopyrightHeaderText, &fileToInsertInto);
    if(!ret)
    {
        emit e("failed to insert copyright header into file: " + absoluteFilePathOfFileToInsertCopyrightHeaderInto);
    }
    return ret;
}
bool D3faultPublicLicenseCopyrightHeaderBulkPrepender::insertFormattedCopyrightHeaderIntoIoDeviceAtLine(uint lineToInsertAt, const QString &formattedCopyrightHeaderText, QIODevice *ioDeviceToInsertCopyrightHeaderInto)
{
    if(ioDeviceToInsertCopyrightHeaderInto->isSequential())
    {
        emit e("can't prepend to sequential device");
        return false;
    }
    if(!ioDeviceToInsertCopyrightHeaderInto->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit e("failed to open file for reading");
        return false;
    }
    QTextStream textStream(ioDeviceToInsertCopyrightHeaderInto);

    QStringList linesSkipped;
    int linesSkippedLength = 0;
    for(uint i = 0; i < lineToInsertAt; ++i)
    {
        QString skippedLine = textStream.readLine();
        linesSkippedLength += skippedLine.length(); //TODOreq: isn't this short 1 for the newline? how did it work before :-S...
        linesSkipped.append(skippedLine);
    }
    if(!textStream.seek(0))
    {
        emit e("failed to seek to beginning of file");
        return false;
    }
    QString allFileData = textStream.readAll();
    QString allFileDataExceptSkippedLines = allFileData.right(allFileData.length() - linesSkippedLength);
    QString newFileData;
    foreach(const QString &lineSkipped, linesSkipped)
    {
        newFileData += lineSkipped + "\n";
    }
    newFileData += formattedCopyrightHeaderText;
    newFileData += allFileDataExceptSkippedLines;

    if(!textStream.seek(0))
    {
        emit e("failed to seek to beginning of file");
        return false;
    }
    ioDeviceToInsertCopyrightHeaderInto->close();
    if(!ioDeviceToInsertCopyrightHeaderInto->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        emit e("failed to open file for writing");
        return false;
    }
    textStream << newFileData;
    ioDeviceToInsertCopyrightHeaderInto->close();
    return true;
}
void D3faultPublicLicenseCopyrightHeaderBulkPrepender::bulkPrependD3faultPublicLicenseCopyrightHeader(const QString &dirToPrependTo, const QString &optionalAlternateCopyrightHeaderFilePath)
{
    emit bulkPrependD3faultPublicLicenseCopyrightHeaderFinished(bulkPrependD3faultPublicLicenseCopyrightHeaderPrivate(dirToPrependTo, optionalAlternateCopyrightHeaderFilePath));
}
