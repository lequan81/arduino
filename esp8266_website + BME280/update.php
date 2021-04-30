<?php
if(isset($_GET['temp']) && isset($_GET['humi']) && isset($_GET['pres']) && isset($_GET['alt'])){
    $temp = $_GET['temp'];
    $humi = $_GET['humi'];
    $pres = $_GET['pres'];
    $alt = $_GET['alt'];
    $b = array(
            'temp' => $temp, 
            'humi' => $humi,
            'pres' => $press,
            'alt' => $alt
    );
    $filedata = fopen("data.json", "w");
    if( $filedata == false )
    {
    	echo "error make file ";
    	exit();
    }
    $data = json_encode($b);
    fwrite($filedata, $data );
    fclose($filedata);
    echo($data);
    }
else{
    echo "no data";
}
?>