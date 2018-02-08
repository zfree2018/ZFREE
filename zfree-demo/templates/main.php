<!DOCTYPE html>
<html>
<head>

<title>PHP-Proxy</title>

<meta name="generator" content="php-proxy.com">
<meta name="version" content="<?=$version;?>">

<style type="text/css">
html body {
	font-family: Arial,Helvetica,sans-serif;
	font-size: 12px;
}

#container {
	width:500px;
	margin:0 auto;
	margin-top:150px;
}

#error {
	color:red;
	font-weight:bold;
}

#frm {
	padding:10px 15px;
	background-color:#FFC8C8;
	
	border:1px solid #818181;
	
	-webkit-border-radius: 8px;
	-moz-border-radius: 8px;
	border-radius: 8px;
}

#footer {
	text-align:center;
	font-size:10px;
	margin-top:35px;
	clear:both;
}
</style>

</head>

<body>


<div id="container">

	<div style="text-align:center;">
		<h1 style="color:blue;">Visit websites through our ISP</h1>
	</div>
	
	<?php if(isset($error_msg)){ ?>
	
	<div id="error">
		<p><?php echo $error_msg; ?></p>
	</div>
	
	<?php } ?>
	
	<div id="frm">
	
	<!-- I wouldn't touch this part -->
	
		<form action="index.php" method="post" style="margin-bottom:0;" id="proxy-form">
			<input id="input-url" name="url" type="text" style="width:400px;" autocomplete="off" placeholder="http://" />
			<input type="submit" value="Go" />
		</form>
		
		<script type="text/javascript">
			document.getElementsByName("url")[0].focus();
		</script>
		
	<!-- [END] -->
		<p class="">
		This is a demo website of our secure zero-rating framework (ZFree).  When you input a URL in the above URL form, 
		we will redirect your traffic through our ISP, i.e., a web proxy.  We create a demo billing system in our ISP. That is,
		 when you visit our uncharged websites, you will get free, uncharged traffic; otherwise, when you visit normal websites, 
		 all your traffic will be charged. 
        </p>
		<button type="button" id="visit-our-server">Visit Uncharged Website 1</button>
		<button type="button" id="visit-our-server2">Visit Uncharged Website 2</button>
		<button type="button" id="visit-google">Visit Charged Website (Google)</button>
		
	</div>
	
</div>

<div id="footer">
	Powered by <a href="//www.php-proxy.com/" target="_blank">PHP-Proxy</a> <?php echo $version; ?>
</div>

<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
<script>
	var base_url = "10.0.0.10";
	$("#visit-our-server").click(function(){
		$("#input-url").val(base_url + "/demo1");
		$("#proxy-form").submit();
	});
	$("#visit-our-server2").click(function(){
		$("#input-url").val(base_url + "/demo2");
		$("#proxy-form").submit();
	});
	$("#visit-google").click(function(){
		$("#input-url").val("www.google.com");
		$("#proxy-form").submit();
	});

</script>

</body>
</html>
