<?php
/* ARS 20170418 - скрипт сканирует каталог с картинками и отдает в браузер последний по алфавиту файл.
                  если задан size, то масштабируем до него. работаем с jpg, png, gif  */

$dir = '/var/www/terrarium-jpg/';
$lastfile = chr(0);

/*------------------------------------------------------------*/
// Open a directory, and read its contents
if (is_dir($dir)){
  if ($dh = opendir($dir)){
    while (($file = readdir($dh)) !== false){
      if ( ($lastfile < $file) && (preg_match("/\.(jpg|jpeg|png|gif|)$/i", $file)) ) { $lastfile = $file; }
    }
    closedir($dh);
  }
}
/*------------------------------------------------------------*/
    if ($lastfile !== chr(0)) { $file = $dir . $lastfile; }
                 else { $file = 'error.jpg';}

    list($width, $height, $image_type) = getimagesize($file);

    switch ($image_type)
    {
        case 1: header('Content-Type: image/gif');  break;
        case 2: header('Content-Type: image/jpeg'); break;
        case 3: header('Content-Type: image/png');  break;
        default: echo 'unknown type'; exit; break;
    }
/*------------------------------------------------------------*/
    if ( isset($_REQUEST['size'] )) { //если задан параметр size=NNN то масштабиркем картинку до этого размера
        $max_width =  $_REQUEST['size'];
        $max_height = $_REQUEST['size'];
    }
    else // иначе просто отдаем файл "как он есть"
    {
        $fp = fopen($file, 'rb');
        header("Content-Length: " . filesize($file));
        // dump the picture and stop the script
        fpassthru($fp);
        exit;
    }
/*------------------------------------------------------------*/
    switch ($image_type)
    {
        case 1: $src = imagecreatefromgif($file); break;
        case 2: $src = imagecreatefromjpeg($file);  break;
        case 3: $src = imagecreatefrompng($file); break;
        default: return '';  break;
    }
/*------------------------------------------------------------*/
    $x_ratio = $max_width / $width;
    $y_ratio = $max_height / $height;

    if( ($width <= $max_width) && ($height <= $max_height) ){
        $tn_width = $width;
        $tn_height = $height;
        }elseif (($x_ratio * $height) < $max_height){
            $tn_height = ceil($x_ratio * $height);
            $tn_width = $max_width;
        }else{
            $tn_width = ceil($y_ratio * $width);
            $tn_height = $max_height;
    }

    $tmp = imagecreatetruecolor($tn_width,$tn_height);

    /* Check if this image is PNG or GIF, then set if Transparent*/
    if(($image_type == 1) OR ($image_type==3))
    {
        imagealphablending($tmp, false);
        imagesavealpha($tmp,true);
        $transparent = imagecolorallocatealpha($tmp, 255, 255, 255, 127);
        imagefilledrectangle($tmp, 0, 0, $tn_width, $tn_height, $transparent);
    }
    imagecopyresampled($tmp,$src,0,0,0,0,$tn_width, $tn_height,$width,$height);
/*------------------------------------------------------------*/
    ob_start();

     switch ($image_type)
    {
        case 1:  imagegif($tmp); break;
        case 2:  imagejpeg($tmp, NULL, 100);  break; // best quality
        case 3:  imagepng($tmp, NULL, 0); break; // no compression
        default: echo ''; break;
    }
    imagedestroy($tmp);
?>
