<html>
 <body>
<?php
	$servername = "mssql1401.wc2\inst1";
	$username = "475692_request";
	$password = "Password123";
	$db = "475692_dbce491";

	// connect to database
	$dbhandle = mssql_connect($servername, $username, $password)
	or die("Couldn't connect to SQL Server on $servername");

	// select a database
	$selected = mssql_select_db($db, $dbhandle)
	or die("Couldn't open database $db"); 	

	echo "Connected<br>";
	
	// run a query
	/*$strSQL = "SELECT U.Name, E.Event, E.Approved, E.Room, E.StartTime, E.EndTime 
			   FROM tbl_Users AS U
			   RIGHT JOIN tbl_Events AS E
			   ON U.RFID = E.RFID
			   WHERE E.StartTime >= '2016-02-05'
			   ORDER BY E.StartTime"; // WHERE E.Approved = 0
	*/
	strSQL = "SELECT U.Name, E.Event, E.Approved, E.Room, E.StartTime, E.EndTime 
			  FROM tbl_Users AS U
			  RIGHT JOIN tbl_Events AS E
			  ON U.RFID = E.RFID
			  WHERE E.Approved = 1
			  ORDER BY E.StartTime"; // WHERE E.Approved = 0
	$query = mssql_query($strSQL);
	
	echo "Queried: ";
	
	// count the returned rows
	$numRows = mssql_num_rows($query);
	if (!$numRows) {
		echo "No records found.";
	} else {
		echo $numRows . " row" . ($numRows == 1 ? "" : "s") . " returned";

		$data = mssql_fetch_array($query);
		$access = $data["Approved"]
		
		if ($access) {
			$name = $data["Name"]
			$event = $data["Event"]
			$room = $data["Room"]
			$startTime = $data["StartTime"]
			$endTime = $data["EndTime"]
			
			//$newtimestamp = strtotime("+20 days", $yourtimestamp);
			
			echo "Welcome " . $name . "! You have room "  $room " scheduled for a " . $event . 
				" meeting from " . $startTime . " to " . $endTime 
		}
	}

	echo "<br>Hello $user!<br>";
?>
 </body>
</html>
