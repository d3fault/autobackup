<?php

//Do not use this script on a Windows server. Windows is a piece of shit because Bill Gates is a noob.

//MANDATORY CONFIG
define('ABC_PHP_CLIENT_USER', 'Replace-With-Your-Username');
define('ABC_PHP_CLIENT_AD_INDEX', 'Replace-With-Your-Ad-Index');
define('ABC_PHP_CLIENT_APIKEY', 'Replace-With-Your-API-Key');
define('ABC_PHP_CLIENT_NO_AD_PLACEHOLDER', 'no.ad.placeholder.jpg'); //When nobody has purchased the ad space, display this image instead

//OPTIONAL CONFIG
define('ABC_PHP_CLIENT_AD_FILENAME_PREFIX', 'ad');
define('ABC_PHP_CLIENT_NO_AD_HOVERTEXT', 'Click here to purchase this ad space'); 
define('ABC_PHP_CLIENT_TEMPORARY_DIR', ''); //Leave blank to use the system temporary dir
define('ABC_PHP_CLIENT_SETTINGS_FILE', 'abcphpclientsettings.json');

//internal
define('ABC_PHP_CLIENT_SETTINGS_FILE_TODAYS_AD_IMAGE_FILENAME_KEY', 'todaysAdImageFilename');
define('ABC_PHP_CLIENT_UPDATE_LOCK_FILENAME', 'abc.api.php.client.update.lock');

function abcApiGenerateCurrentAdHtml($adImageFilename, $adUrl, $adHoverText)
{
	echo '<a href="' . $adUrl . '"><img width="576" height="96" src="' . $adImageFilename . '" alt="' . $adHoverText . '" title="' . $adHoverText . '" /></a>';
}

function abcApiReturnBuyAdSpaceUrl()
{
	return 'https://anonymousbitcoincomputing.com/advertising/buy-ad-space/' . ABC_PHP_CLIENT_USER . '/' . ABC_PHP_CLIENT_AD_INDEX;
}

function abcApiGenerateNoAdHtml()
{
	abcApiGenerateCurrentAdHtml(ABC_PHP_CLIENT_NO_AD_PLACEHOLDER, abcApiReturnBuyAdSpaceUrl(), ABC_PHP_CLIENT_NO_AD_HOVERTEXT);
}

function myTempDir()
{
	if(ABC_PHP_CLIENT_TEMPORARY_DIR == '')
	{
        	return sys_get_temp_dir();
	}
	return ABC_PHP_CLIENT_TEMPORARY_DIR;
}

function myTempFile()
{	
	return tempnam(myTempDir(), 'ABC');
}

function atomicallyCreateSettingsFileForNewAd($adImageFilename, $adUrl, $adHoverText, $adSlotExpireDateTime)
{
	$tempSettingsFile = myTempFile();
	if($tempSettingsFile === FALSE)
	{
		//What can we do? Just exitting like this might cause API flooding. Maybe I should try to write the settings file in place as a desperate measure? TODOreq. A common source of this is permission errors. Maybe at the VERY BEGINNING of the script, I check that the temp dir we'll use is writeable, and then exit if it isn't (so no API query flood)
		return;
	}
	$tempSettingsFileHandle = fopen($tempSettingsFile, 'w');
	if(!$tempSettingsFileHandle)
	{
		//Ditto above API flooding because of permissions
		return;
	}
	$settingsArray = array(ABC_PHP_CLIENT_SETTINGS_FILE_TODAYS_AD_IMAGE_FILENAME_KEY => $adImageFilename, 'urlB64' => base64_encode($adUrl), 'hoverTextB64' => base64_encode($adHoverText), 'todaysAdSlotExpireDateTime' => $adSlotExpireDateTime);
	fwrite($tempSettingsFileHandle, json_encode($settingsArray));
	fflush($tempSettingsFileHandle);
	fclose($tempSettingsFileHandle);
	if(!rename($tempSettingsFile, ABC_PHP_CLIENT_SETTINGS_FILE))
	{
		//Ditto above API flooding because of permissions
		return;
	}
}

function createSettingsFileInNoAdMode()
{
	//TO DOnereq: the settings file whenever the ad slot isn't purchased and we want to poll every 5 minutes. One way to do this is to simply have it the settings file not exist? Another is to fill in the fields correctly, but use 'no ad placeholder' image and set expiration date 5 mins in future. I think the latter is better, because it not existing would mean EVERY time the script runs, an API request would be made xD
	atomicallyCreateSettingsFileForNewAd(ABC_PHP_CLIENT_NO_AD_PLACEHOLDER, abcApiReturnBuyAdSpaceUrl(), ABC_PHP_CLIENT_NO_AD_HOVERTEXT, (time() + 300));
}

function queryAbcApi($adImageFilenameToShowIfWeFailToGetUpdateLock, $adUrlToUseIfWeFailToGetUpdateLock, $adHoverTextToUseIfWeFailToGetUpdateLock)
{
	$updateLockHandle = fopen(ABC_PHP_CLIENT_UPDATE_LOCK_FILENAME, 'c');
	if(!$updateLockHandle)
	{
		//Fatal error, should never happen
		abcApiGenerateCurrentAdHtml($adImageFilenameToShowIfWeFailToGetUpdateLock, $adUrlToUseIfWeFailToGetUpdateLock, $adHoverTextToUseIfWeFailToGetUpdateLock);
		exit;
	}
	if(!flock($updateLockHandle, LOCK_EX | LOCK_NB))
	{
		//Failed to get update lock, which means another instance of this script is currently doing the update. So just show whatever we can
		abcApiGenerateCurrentAdHtml($adImageFilenameToShowIfWeFailToGetUpdateLock, $adUrlToUseIfWeFailToGetUpdateLock, $adHoverTextToUseIfWeFailToGetUpdateLock);
		exit;
	}

	$apiResponse = file_get_contents('https://anonymousbitcoincomputing.com:420/api?action=getcurrentad&user=' . ABC_PHP_CLIENT_USER . '&index=' . ABC_PHP_CLIENT_AD_INDEX . '&apikey=' . ABC_PHP_CLIENT_APIKEY);
	if($apiResponse === FALSE)
	{
        	//Unable to query API, so use the no ad placeholder
		createSettingsFileInNoAdMode();
        	abcApiGenerateNoAdHtml();
		flock($updateLockHandle, LOCK_UN);
        	exit;
	}
	//$apiResponse = utf8_encode($apiResponse);
	$apiResponseJson = json_decode($apiResponse);
	if(($apiResponseJson === NULL) || ($apiResponseJson === FALSE))
	{
        	//Failed to parse JSON, so use the no ad placeholder
	        createSettingsFileInNoAdMode();
        	abcApiGenerateNoAdHtml();
		flock($updateLockHandle, LOCK_UN);
        	exit;
	}
	if($apiResponseJson->{'error'} != 'none')
	{
        	//The ad slot is not currently purchased, OR there was a JSON error, so use the no ad placeholder
	        createSettingsFileInNoAdMode();
		abcApiGenerateNoAdHtml();
		flock($updateLockHandle, LOCK_UN);
		exit;
	}
	$adImage = base64_decode($apiResponseJson->{'adImageB64'});
	$adHoverText = base64_decode($apiResponseJson->{'hoverTextB64'});
	$adUrl = base64_decode($apiResponseJson->{'urlB64'});
	if($adImage === FALSE || $adHoverText === FALSE || $adUrl === FALSE)
	{
        	//Failed to decode base64, so use the no ad placeholder
	        createSettingsFileInNoAdMode();
		abcApiGenerateNoAdHtml();
		flock($updateLockHandle, LOCK_UN);
        	exit;
	}
	$tempFile = myTempFile();
	if($tempFile === FALSE)
	{
        	//Failed to create temp file, so use the no ad placeholder
	        createSettingsFileInNoAdMode();
		abcApiGenerateNoAdHtml();
		flock($updateLockHandle, LOCK_UN);
        	exit;
	}
	$tempFileHandle = fopen($tempFile, 'wb');
	if($tempFileHandle === FALSE)
	{
        	//Failed to open temp file for writing, so use the no ad placeholder
	        createSettingsFileInNoAdMode();
		abcApiGenerateNoAdHtml();
		flock($updateLockHandle, LOCK_UN);
        	exit;
	}
	$numBytesWritten = fwrite($tempFileHandle, $adImage);
	if($numBytesWritten === FALSE)
	{
        	//Failed to write ad image to file, so use the no ad placeholder
	        createSettingsFileInNoAdMode();
		abcApiGenerateNoAdHtml();
		flock($updateLockHandle, LOCK_UN);
        	exit;
	}
	fflush($tempFileHandle);
	fclose($tempFileHandle);
	$adImageFilename = ABC_PHP_CLIENT_AD_FILENAME_PREFIX . $apiResponseJson->{'adImageExt'};
	if(!rename($tempFile, $adImageFilename))
	{
        	//Failed to rename, so use the no ad placeholder
	        createSettingsFileInNoAdMode();
		abcApiGenerateNoAdHtml();
		flock($updateLockHandle, LOCK_UN);
        	exit;
	}
	atomicallyCreateSettingsFileForNewAd($adImageFilename, $adUrl, $adHoverText, $apiResponseJson->{'todaysAdSlotExpireDateTime'});
	flock($updateLockHandle, LOCK_UN);
	abcApiGenerateCurrentAdHtml($adImageFilename, $adUrl, $adHoverText);
}

function queryAbcUsingNoAdPlaceholderAsFallback()
{
	queryAbcApi(ABC_PHP_CLIENT_NO_AD_PLACEHOLDER, abcApiReturnBuyAdSpaceUrl(), ABC_PHP_CLIENT_NO_AD_HOVERTEXT);
}


//int main()
if((!is_writable(myTempDir())) || (!is_writable(dirname(__FILENAME__))))
{
	exit; //an error message would be displayed in the html lol? so just exit...
}
$settingsFileContents = file_get_contents(ABC_PHP_CLIENT_SETTINGS_FILE);
if($settingsFileHandle === FALSE)
{
	//first script run, settings file doesn't exist, so we definitely have to query the abc api (which creates the settings file)
	queryAbcUsingNoAdPlaceholderAsFallback();
	exit;
}
$settingsFileJson = json_decode($settingsFileContents);
if(($settingsFileJson === NULL) || ($settingsFileJson === FALSE))
{
        //Failed to parse settings json. This should never happen. We query the abc api so that the settings file gets recreated (properly)
        //queryAbcApi(ABC_PHP_CLIENT_NO_AD_PLACEHOLDER);
	queryAbcUsingNoAdPlaceholderAsFallback();
        exit;
}
if(time() >= $settingsFileJson->{'todaysAdSlotExpireDateTime'})
{
	//Expired. Try to get new, but if we fail to get the update lock, we'll still show the expired ad
	queryAbcApi($settingsFileJson->{ABC_PHP_CLIENT_SETTINGS_FILE_TODAYS_AD_IMAGE_FILENAME_KEY}, base64_decode($settingsFileJson->{'urlB64'}), base64_decode($settingsFileJson->{'hoverTextB64'}));
        exit;
}
//Not expired (typical use case)
abcApiGenerateCurrentAdHtml($settingsFileJson->{ABC_PHP_CLIENT_SETTINGS_FILE_TODAYS_AD_IMAGE_FILENAME_KEY}, base64_decode($settingsFileJson->{'urlB64'}), base64_decode($settingsFileJson->{'hoverTextB64'}));
?>
