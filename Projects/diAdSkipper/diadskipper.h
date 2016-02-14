#ifndef DIADSKIPPER_H
#define DIADSKIPPER_H

#include <QObject>
#include <QProcess>
#include <QTextStream>
#include <QTimer>
#include <QStringList>
#include <QCoreApplication>
#include <QFile>

class DiAdSkipper : public QObject
{
Q_OBJECT
public:
	DiAdSkipper(QStringList arguments, QObject *parent = 0)
		: QObject(parent), m_ExtraWaitSeconds(13), m_IcyInfoBlacklistLocation("icyinfoblacklist.txt"), m_MaxWaitSeconds(300), m_MplayerPath("/usr/bin/mplayer"), m_MplayerTextStream(&m_MplayerProcess), m_StdIn(stdin), m_StdOut(stdout), m_WeMutedForAd(false)
	{
		connect(m_StdIn.device(), SIGNAL(readyRead()), this, SLOT(handleUserInput()));
		m_SleepAfterQuitLengthMs = 60000;
		if(arguments.size() < 2)
		{
			usage();
			return;
		}
		m_RadioUrl = arguments.at(1);
		if(arguments.size() > 2)
		{
			bool convertOk = false;
			int extraWait = arguments.at(2).toInt(&convertOk);
			if(!convertOk)
			{
				m_StdOut << "Error parsing extra wait time" << endl;
				usage();
				return;
			}
			m_ExtraWaitSeconds = extraWait;

			if(arguments.size() > 3)
			{
				m_IcyInfoBlacklistLocation = arguments.at(3);
				if(!QFile::exists(m_IcyInfoBlacklistLocation))
				{
					m_StdOut << "Error, the blacklist you specified could not be found" << endl;
					usage();
					return;
				}

				if(arguments.size() > 4)
				{
					int maxWait = arguments.at(4).toInt(&convertOk);
					if(!convertOk)
					{
						m_StdOut << "Error parsing max wait time" << endl;
						usage();
						return;
					}
					m_MaxWaitSeconds = maxWait;

					if(arguments.size() > 5)
					{
						m_MplayerPath = arguments.at(5);
						if(!QFile::exists(m_MplayerPath))
						{
							m_StdOut << "Error, the path to mplayer you specified does not exist" << endl;
							usage();
							return;
						}
					}
				}
			}
		}
		m_MplayerProcess.setProcessChannelMode(QProcess::MergedChannels);
		m_MplayerProcess.setReadChannel(QProcess::StandardOutput);
		connect(&m_MplayerProcess, SIGNAL(readyRead()), this, SLOT(handleMplayerStdOutHasData()));
		connect(&m_MplayerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleMplayerFinished()));
		connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
		readBlacklistFile();
		//m_StdOut << "Ctrl+C to stop..." << endl;
		m_StdOut.flush();
		startMplayer();
	}
private:
	int m_SleepAfterQuitLengthMs;
	QString m_RadioUrl;
	int m_ExtraWaitSeconds;
	QString m_IcyInfoBlacklistLocation;
	int m_MaxWaitSeconds;
	QString m_MplayerPath;
	QStringList m_Blacklist;
	void readBlacklistFile()
	{
		QFile blacklistFile(m_IcyInfoBlacklistLocation);
		if(blacklistFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream blacklistTextStream(&blacklistFile);
			while(!blacklistTextStream.atEnd())
			{
				QString currentLine = blacklistTextStream.readLine();
				m_Blacklist << currentLine;
			}
		}
		//else: create and explain to user how to modify
	}
	void usage()
	{
		m_StdOut << "MplayerIcyMuter usage (square backets denote optional arguments): " << endl << endl;
		m_StdOut << "MplayerIcyMuter urlToStream [extraWaitTime=13] [icyinfoblacklist.txt] [maxWaitTime=300] [pathToMplayer=/usr/bin/mplayer]" << endl << endl << endl;
		m_StdOut << "Examples:" << endl << endl;
		m_StdOut << "Minimal:" << endl;
		m_StdOut << "MplayerIcyMuter \"http://example.com:420/radio_station_url\"" << endl << endl;
		m_StdOut << "Specify zero extra seconds extra wait to unmute when a non-blacklisted 'ICY Info' line is seen, alternate blacklist location, 15 seconds max to unmute, and an alternate mplayer location:" << endl;
		m_StdOut << "MplayerIcyMuter \"http://example.com:420/radio_station_url\" 0 \"c:/path/to/icy/info/blacklist.txt\" 15 \"c:/path/to/mplayer.exe\"" << endl;
		QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
		m_StdOut.flush();
	}
	void startMplayer()
	{
		QStringList mplayerArgs;
		mplayerArgs << m_RadioUrl;
		m_MplayerProcess.start(m_MplayerPath, mplayerArgs);
	}
	QProcess m_MplayerProcess;
	QTextStream m_MplayerTextStream;
	QTextStream m_StdIn;
	QTextStream m_StdOut;
	bool m_WeMutedForAd;
	void muteAudio()
	{
		QStringList muteArgs;
		muteArgs << "sset" << "PCM" << "0%";
		QProcess::execute("/usr/bin/amixer", muteArgs);
		m_WeMutedForAd = true;
	}
	void unMuteAudio()
	{
		if(!m_WeMutedForAd) //don't unmute if not muted (5 min timeout may try this mainly)
			return;
		QStringList unMuteArgs;
		unMuteArgs << "sset" << "PCM" << "99%";
		QProcess::execute("/usr/bin/amixer", unMuteArgs);
		m_WeMutedForAd = false;
		//m_MplayerTextStream << 'q'; //hack, idfk
		m_SleepAfterQuitLengthMs = 1000;
		m_MplayerProcess.terminate();
	}
	bool currentLineContainsBlacklistedString(const QString &currentLine)
	{
		if(currentLine.contains("_ad='true'") || currentLine.contains("Choose premium for the best audio experience") || currentLine.contains("There's more to Digitally Imported!") || currentLine.contains("Digitally Imported TSTAG_60 ADWTAG") || currentLine.contains("More of the show after these messages") || currentLine.contains("Get Digitally Imported Premium"))
			return true;
		return false;
	}
private slots:
	void handleUserInput()
	{
		QString currentLine = m_StdIn.readLine();
		if(currentLine.toLower().contains("q"))
		{
			QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
		}
	}
	void doStartMplayer()
	{
		startMplayer();
	}
	void handleMplayerStdOutHasData()
	{
		QString currentLine = m_MplayerTextStream.readLine();
		if(!currentLine.startsWith("A:"))
		{
			if(currentLine.trimmed().isEmpty())
				return;
			m_StdOut << currentLine << endl;
			m_StdOut.flush();
			if(!currentLine.startsWith("ICY Info:"))
				return;
			if(currentLineContainsBlacklistedString(currentLine))
			{
				if(m_WeMutedForAd) //don't double mute
					return;
				muteAudio();
				QTimer::singleShot((m_MaxWaitSeconds*1000), this, SLOT(doUnmute())); //sometimes it doesn't unmute wtf? so after 5 mins we unmute no matter what
				m_StdOut << "...Muting For Advertisement..." << endl;
				m_StdOut.flush();
				return;
			}
			if(m_WeMutedForAd) //transition from ad -> music, which generally has 13 seconds of ad overlapping
				QTimer::singleShot((m_ExtraWaitSeconds*1000), this, SLOT(doUnmute()));
			//m_StdOut << currentLine << endl;
			//m_StdOut.flush();
		}

	}
	void doUnmute()
	{
		unMuteAudio();
	}
	void handleMplayerFinished()
	{
		//since we're listening to a stream, it should never finish... and yet it does sometimes (buggy network, etc). So if it finishes and we didn't tell it to, start it back up after giving it a minute so routers etc can restart :)
		//m_SleepAfterQuitLengthMs = 60000;
		m_StdOut << "Mplayer finished, but we didn't tell it to. Restarting in some seconds" << endl;
		m_StdOut.flush();
		QTimer::singleShot(m_SleepAfterQuitLengthMs, this, SLOT(doStartMplayer()));
		m_SleepAfterQuitLengthMs = 60000;
	}
	void handleAboutToQuit()
	{
		disconnect(&m_MplayerProcess, SIGNAL(finished(int,QProcess::ExitStatus)));
		m_MplayerTextStream << 'q';
		//m_MplayerProcess.terminate();
		m_StdOut << "Waiting for Mplayer to Finish... ";
		m_MplayerProcess.waitForFinished();
		m_StdOut << "Done" << endl;
		m_StdOut.flush();
	}
};

#endif
