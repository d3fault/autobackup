<?php

//TODOreq: caching

//MANDATORY CONFIG
define('ABC_PHP_CLIENT_USER', 'Replace-With-Your-Username');
define('ABC_PHP_CLIENT_AD_INDEX', 'Replace-With-Your-Ad-Index');
define('ABC_PHP_CLIENT_APIKEY', 'Replace-With-Your-API-Key');
define('ABC_PHP_CLIENT_NO_AD_PLACEHOLDER', '/home/user/temp/no.ad.placeholder.jpg'); //When nobody has purchased the ad space, display this image instead

//OPTIONAL CONFIG
define('ABC_PHP_CLIENT_AD_FILENAME', 'ad.jpg'); //The image filename to be embedded using the <img> tag in a static index.html (etc) file
define('ABC_PHP_CLIENT_TEMPORARY_DIR', ''); //Leave blank to use the temporary dir that sys_get_temp_dir gives us


$apiResponse = file_get_contents('https://anonymousbitcoincomputing.com:420/api?action=getcurrentad&user=' . ABC_PHP_CLIENT_USER . '&index=' . ABC_PHP_CLIENT_AD_INDEX . '&apikey=' . ABC_PHP_CLIENT_APIKEY);
if($apiResponse === FALSE)
{
        //Unable to query API, so use the no ad placeholder TODOreq
        echo "Failed to query API\n";
        exit;
}
//$apiResponse = utf8_encode($apiResponse);
$apiResponseJson = json_decode($apiResponse);
if(($apiResponseJson === NULL) || ($apiResponseJson === FALSE))
{
        //Failed to parse JSON, so use the no ad placeholder TODOreq
        echo "Failed to parse JSON\n";
        exit;
}
if($apiResponseJson->{'error'} != 'none')
{
        //JSON had error, so use the no ad placeholder TODOreq
        echo 'API response had error: ' . $apiResponseJson->{'error'} . "\n";
        exit;
}
$adImage = base64_decode($apiResponseJson->{'adImageB64'});
$adHoverText = base64_decode($apiResponseJson->{'hoverTextB64'});
$adUrl = base64_decode($apiResponseJson->{'urlB64'});
//TODOreq: use ad image extension
if($adImage === FALSE || $adHoverText === FALSE || $adUrl === FALSE)
{
        //Failed to decode base64, so use the no ad placeholder TODOreq
        echo "Failed to decode base64 data\n";
        exit;
}
$tempDirToUse = ABC_PHP_CLIENT_TEMPORARY_DIR;
if(ABC_PHP_CLIENT_TEMPORARY_DIR == '')
{
        $tempDirToUse = sys_get_temp_dir();
}
$tempFile = tempnam($tempDirToUse, 'ABC');
if($tempFile === FALSE)
{
        //Failed to create temp file, so use the no ad placeholder TODOreq
        echo 'Failed to create temp file: ' . $tempFile . "\n";
        exit;
}
$tempFileHandle = fopen($tempFile, 'wb');
if($tempFileHandle === FALSE)
{
        //Failed to open temp file for writing, so use the no ad placeholder TODOreq
        echo 'Failed to open temp file for writing: ' . $tempFile . "\n";
        exit;
}
$numBytesWritten = fwrite($tempFileHandle, $adImage);
if($numBytesWritten === FALSE)
{
        //Failed to write ad image to file, so use the no ad placeholder TODOreq
        echo 'Failed to write ad image to file: ' . $tempFile . "\n";
        exit;
}
fclose($tempFileHandle);
if(!rename($tempFile, ABC_PHP_CLIENT_AD_FILENAME))
{
        //Eh if the rename fails, so too will the renaming used to set up the no ad placeholder TODOreq
        echo 'Failed to rename ' . $tempFile . ' to ' . ABC_PHP_CLIENT_AD_FILENAME . "\n";
        exit;
}
echo '<a href="' . $adUrl . '"><img src="' . ABC_PHP_CLIENT_AD_FILENAME . '" alt="' . $adHoverText . '" title="' . $adHoverText . '" /></a>';
?>
