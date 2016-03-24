<?php
	if ($_GET) {
		$argument1 = $_GET['argument1'];
		$argument1 = $_GET['argument1'];
	} else {
		$rfidValue = $argv[2];
		$timeRequest = $argv[3];
		$roomRequest = $argv[1];
	}
	
	$servername = "172.99.97.197:4120";
	$username = "475692_request";
	$password = "Password123";
	$db = "475692_dbce491";
	// connect to database
	$dbhandle = mssql_connect($servername, $username, $password)
	or die("Couldn't connect to SQL Server on $servername");
	// select a database
	$selected = mssql_select_db($db, $dbhandle)
	or die("Couldn't open database $db"); 	
	
	//$rfidValue = "18460";
	//$roomRequest = "206"
	$timeRequest = str_replace("_", " ", $timeRequest);
	//$timeRequest = strtotime("+20 minutes", $timeRequest);
	
	$strSQL = "SELECT U.Name, U.RFID, E.Event, E.Approved, E.Room, E.StartTime, E.EndTime
			   FROM tbl_Users AS U
			   RIGHT JOIN tbl_Events AS E
			   ON U.RFID = E.RFID
			   WHERE U.RFID = " . $rfidValue . "
			   AND E.Room = '" . $roomRequest . "'
			   AND E.StartTime < '" . $timeRequest . "' 
			   AND E.EndTime > '" . $timeRequest . "'";
	$query = mssql_query($strSQL);
	// count the returned rows
	$numRows = mssql_num_rows($query);
	
	if (!$numRows) {
		echo "Access denied.";
	} 
	else if($numRows) {
		echo "Access granted";
	}
	else {
		$data = mssql_fetch_array($query);
		$access = $data["Approved"];
		
		if ($access) {
			$name = $data["Name"];
			$event = $data["Event"];
			$room = $data["Room"];
			$startTime = $data["StartTime"];
			$endTime = $data["EndTime"];
			
			
			echo "Welcome " . $name . "! You have room " . $room . " scheduled for a " . $event . 
				" meeting from " . $startTime . " to " . $endTime;
		}
	}
?>
