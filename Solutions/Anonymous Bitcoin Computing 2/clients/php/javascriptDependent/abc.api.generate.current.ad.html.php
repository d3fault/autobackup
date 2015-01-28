<?php

//Do not use this script on a Windows server. Windows is a piece of shit because Bill Gates is a noob. This script relies on atomic move/rename and non-blocking file lock aquisition

//MANDATORY CONFIG
define('ABC_PHP_CLIENT_USER', 'Replace-With-Your-Username'); //Your Anonymous Bitcoin Computing username
define('ABC_PHP_CLIENT_AD_INDEX', 'Replace-With-Your-Ad-Index'); //Your ad index, probably '0' if this is your first ad space for sale
define('ABC_PHP_CLIENT_APIKEY', 'Replace-With-Your-API-Key'); //Your API Key. Get this from the API section of your Account on the Anonymous Bitcoin Computing website
define('ABC_PHP_CLIENT_AD_IMAGE_WIDTH', 'Replace-With-Your-Ad-Image-Width'); //Your ad image width
define('ABC_PHP_CLIENT_AD_IMAGE_HEIGHT', 'Replace-With-Your-Ad-Image-Height'); //Your ad image height
define('ABC_PHP_CLIENT_NO_AD_PLACEHOLDER', 'no.ad.placeholder.jpg'); //When nobody has purchased the ad space, display this image instead

//OPTIONAL CONFIG
define('ABC_PHP_CLIENT_AD_FILENAME_PREFIX', 'ad');
define('ABC_PHP_CLIENT_NO_AD_HOVERTEXT', 'Click here to purchase this ad space'); 
define('ABC_PHP_CLIENT_TEMPORARY_DIR', ''); //Leave blank to use the system temporary dir
define('ABC_PHP_CLIENT_AD_CACHE_DB_FILE_PREFIX', 'abc.ad.cache.db.for');
define('ABC_PHP_CLIENT_AD_CACHE_DB_FILE_EXT', '.json');

//internal
define('ABC_PHP_CLIENT_AD_CACHE_DB_FILE_TODAYS_AD_IMAGE_FILENAME_JSON_KEY', 'todaysAdImageFilename');
define('ABC_PHP_CLIENT_AD_CACHE_DB_UPDATE_LOCK_FILE_PREFIX', 'abc.api.php.client.update.lock.for.');

function abcApiGenerateCurrentAdHtml($adImageFilename, $adUrl, $adHoverText)
{
	if($_GET['foriframe'] == '1')
	{
		echo '<html><head><body>';
	}
	echo '<a href="' . $adUrl . '"><img width="' . ABC_PHP_CLIENT_AD_IMAGE_WIDTH . '" height="' . ABC_PHP_CLIENT_AD_IMAGE_HEIGHT . '" src="' . $adImageFilename . '" alt="' . $adHoverText . '" title="' . $adHoverText . '" /></a>';
	if($_GET['foriframe'] == '1')
	{
		echo '</body></head></html>';
	}
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
	$ret = ABC_PHP_CLIENT_TEMPORARY_DIR;
	if(ABC_PHP_CLIENT_TEMPORARY_DIR == '')
	{
        	$ret = sys_get_temp_dir();
	}
	if(substr($ret, -1) != '/')
	{
		$ret = $ret . '/';
	}
	return $ret;
}

function myTempFile()
{	
	return tempnam(myTempDir(), 'ABC');
}

function myAdCacheDbFile()
{
        return myTempDir() . ABC_PHP_CLIENT_AD_CACHE_DB_FILE_PREFIX . '.' . ABC_PHP_CLIENT_USER . '.' . ABC_PHP_CLIENT_AD_INDEX . ABC_PHP_CLIENT_AD_CACHE_DB_FILE_EXT;
}

function atomicallyCreateSettingsFileForNewAd($adImageFilename, $adUrl, $adHoverText, $adSlotExpireDateTime)
{
	$tempSettingsFile = myTempFile();
	if($tempSettingsFile === FALSE)
	{
		//What can we do? Just exitting like this might cause API flooding. Maybe I should try to write the settings file in place as a desperate measure? A common source of this is permission errors. Maybe at the VERY BEGINNING of the script, I check that the temp dir we'll use is writeable, and then exit if it isn't (so no API query flood) -- I do have this implemented, but not sure it covers everything to prevent accidental api flooding
		return;
	}
	$tempSettingsFileHandle = fopen($tempSettingsFile, 'w');
	if(!$tempSettingsFileHandle)
	{
		//Ditto above API flooding because of permissions
		return;
	}
	$settingsArray = array(ABC_PHP_CLIENT_AD_CACHE_DB_FILE_TODAYS_AD_IMAGE_FILENAME_JSON_KEY => $adImageFilename, 'urlB64' => base64_encode($adUrl), 'hoverTextB64' => base64_encode($adHoverText), 'todaysAdSlotExpireDateTime' => $adSlotExpireDateTime);
	fwrite($tempSettingsFileHandle, json_encode($settingsArray));
	fflush($tempSettingsFileHandle);
	fclose($tempSettingsFileHandle);
	if(!rename($tempSettingsFile, myAdCacheDbFile()))
	{
		//Ditto above API flooding because of permissions
		return;
	}
}

function createSettingsFileInNoAdMode()
{
	atomicallyCreateSettingsFileForNewAd(ABC_PHP_CLIENT_NO_AD_PLACEHOLDER, abcApiReturnBuyAdSpaceUrl(), ABC_PHP_CLIENT_NO_AD_HOVERTEXT, (time() + 300));
}

function queryAbcApi($adImageFilenameToShowIfWeFailToGetUpdateLock, $adUrlToUseIfWeFailToGetUpdateLock, $adHoverTextToUseIfWeFailToGetUpdateLock)
{
        $updateLockHandle = fopen(myTempDir() . ABC_PHP_CLIENT_AD_CACHE_DB_UPDATE_LOCK_FILE_PREFIX . ABC_PHP_CLIENT_USER . '.' . ABC_PHP_CLIENT_AD_INDEX, 'c');
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
	$adImageFilename = ABC_PHP_CLIENT_AD_FILENAME_PREFIX . '.' . ABC_PHP_CLIENT_USER . '.' . ABC_PHP_CLIENT_AD_INDEX . $apiResponseJson->{'adImageExt'};
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
if(!is_writable(myTempDir()))
{
	echo 'error. directory does not have write permissions: ' . myTempDir() . "\n";
	exit;
}
if(!is_writable(dirname(__FILENAME__)))
{
	echo 'error. directory does not have write permissions: ' . dirname(__FILENAME__) . "\n";
	exit;
}
$settingsFileContents = file_get_contents(myAdCacheDbFile());
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
	queryAbcUsingNoAdPlaceholderAsFallback();
        exit;
}
if(time() >= $settingsFileJson->{'todaysAdSlotExpireDateTime'})
{
	//Expired. Try to get new, but if we fail to get the update lock, we'll still show the expired ad -- if there are any other errors aside from failing to get the update lock, we show the no ad placeholder
	queryAbcApi($settingsFileJson->{ABC_PHP_CLIENT_AD_CACHE_DB_FILE_TODAYS_AD_IMAGE_FILENAME_JSON_KEY}, base64_decode($settingsFileJson->{'urlB64'}), base64_decode($settingsFileJson->{'hoverTextB64'}));
        exit;
}
//Not expired (typical use case) -- cache hit
abcApiGenerateCurrentAdHtml($settingsFileJson->{ABC_PHP_CLIENT_AD_CACHE_DB_FILE_TODAYS_AD_IMAGE_FILENAME_JSON_KEY}, base64_decode($settingsFileJson->{'urlB64'}), base64_decode($settingsFileJson->{'hoverTextB64'}));
?>
