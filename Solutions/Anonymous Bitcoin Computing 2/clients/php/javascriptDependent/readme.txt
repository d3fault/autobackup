This simple PHP API client is the easiest way to get the ads purchased through Anonymous Bitcoin Computing to display on your site. It also serves as an API usage example for those who want a more custom solution, or who want to use a language other than PHP.

CONFIGURE
1) Open the file named 'abc.api.generate.current.ad.html.php' in notepad, or your text editor of choice. At the top you'll see a section called 'MANDATORY CONFIG'. Fill in these 4 necessary values
2) Where it says 'Replace-With-Your-Username', you'll fill in the username that you use to log in to Anonymous Bitcoin Computing
3) Where it says 'Replace-With-Your-Ad-Index', you'll use the index displayed in the address bar when viewing your ad space available for purchase. If this is your first and only ad space, the index will be '0' (zero)
4) Where it says 'Replace-With-Your-API-Key', you'll need to log in to Anonymous Bitcoin Computing and go to the API section of your Account. On that page will be your API Key, just copy/paste it verbatim
5) The last option, already filled in with 'no.ad.placeholder.jpg', indicates what image will be shown whenever the ad space has not been purchased. You should create an image (with dimensions 576x96) to be displayed in that case, and supply it's filename (or name your custom image 'no.ad.placeholder.jpg') in the config
6) Save the file and close it

INSTALL
1) Your webserver needs to have PHP installed. Most cheap webservers already have PHP installed. If you don't have PHP installed, or if you don't know if you have PHP installed, contact your IT administrator
2) FTP transfer, or otherwise send, the file 'abc.api.generate.current.ad.html.php' to your webserver in a place that is publicly accessible. If you're working with HTML files, just put 'abc.api.generate.current.ad.html.php' right next to the HTML file you want the ad to be displayed in. If you're unsure where to put the file, inside the "public_html" folder might be the right place... but you should contact your IT administrator to make sure. The directory must also have write permissions, since it's where the ad image file will be placed.
3) Paste the following HTML snippet into whatever HTML file you want the ad to be displayed in (or wherever you can paste an HTML snippet, if you're using a content management system such as Wordpress)

<-- BEGIN ABC PHP API CLIENT HTML SNIPPET -->
<div id='abc-ad-div-placeholder'></div>
<script language="javascript" type="text/javascript">
	var xmlhttp;
	if(window.XMLHttpRequest)
	{
		xmlhttp = new XMLHttpRequest();
	}
	else
	{
		xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
	}
	xmlhttp.onreadystatechange=function()
	{
		if(xmlhttp.readyState == 4 && xmlhttp.status == 200)
		{
			document.getElementById('abc-ad-div-placeholder').innerHTML = xmlhttp.responseText;
		}
	}
	xmlhttp.open('GET', 'abc.api.generate.current.ad.html.php', true);
	xmlhttp.send();
</script>
<noscript>
	<iframe src='abc.api.generate.current.ad.html.php?foriframe=1' height='96' width='576' frameborder='0' marginwidth='0' scrolling='no'></iframe>
</noscript>
<-- END ABC PHP API CLIENT HTML SNIPPET -->

4) The included index.html file demonstrates the above HTML snippet pasted into a correct place
5) Save the HTML file or submit the snippet or whatever, upload it to your webserver if you need to, and then go visit the site. If nobody has purchased your ad space, you should see the 'no ad placeholder' image you created in step (5) of CONFIGURE. Once someone purchases your ad space, it will take up to five minutes for the advertisement to be displayed (and it will only show up after a page refresh)


How to display multiple ads on the same website:
If you want to display more than one ad on a single website, you need to do some more tweaking.
For every ad you want to display, you need to make a copy of the 'abc.api.generate.current.ad.html.php' file and change it's filename so that it doesn't conflict with the one you copied from. For example, to display two ads on a single website, you could name the files 'abc.api.generate.current.ad.html.1.php' and 'abc.api.generate.current.ad.html.2.php'. The names you choose are arbitrary, but they must not conflict with each other! They would probably be configured with different different ad indexes as well. IMPORTANT: Whatever filename you chose appears TWICE in the HTML snippet, so you need to modify the HTML snippet to reflect your new non-conflicting filenames.
If you want two ads to be displayed in the same HTML file, you need to ALSO change the div-id (which is simply 'abc-ad-div-placeholder' by default) so that it doesn't conflict. For example, two display two ads in a single HTML file, you could change the div-ids to 'abc-ad-div-placeholder1' and 'abc-ad-div-placeholder2'. Make sure you change both occurances of the div-id in the HTML snippet.