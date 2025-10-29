<?php
// CGI headers - CRÍTICO para el funcionamiento
echo "Content-Type: text/html\r\n\r\n";

// En CGI, necesitamos leer los datos de forma diferente
$method = getenv('REQUEST_METHOD');
$nombre = 'Sin nombre';
$email = 'Sin email';
$mensaje = 'Sin mensaje';
$color = 'Sin color';

if ($method == 'POST') {
    // Leer el body del POST desde stdin usando file_get_contents
    $content_length = getenv('CONTENT_LENGTH');
    if ($content_length > 0) {
        $post_data = file_get_contents('php://stdin');
        parse_str($post_data, $post_vars);
        
        $nombre = isset($post_vars['nombre']) ? $post_vars['nombre'] : 'Sin nombre';
        $email = isset($post_vars['email']) ? $post_vars['email'] : 'Sin email';
        $mensaje = isset($post_vars['mensaje']) ? $post_vars['mensaje'] : 'Sin mensaje';
        $color = isset($post_vars['color']) ? $post_vars['color'] : 'Sin color';
    }
} else if ($method == 'GET') {
    // Leer los datos del query string
    $query_string = getenv('QUERY_STRING');
    if ($query_string) {
        parse_str($query_string, $get_vars);
        
        $nombre = isset($get_vars['nombre']) ? $get_vars['nombre'] : 'Sin nombre';
        $email = isset($get_vars['email']) ? $get_vars['email'] : 'Sin email';
        $mensaje = isset($get_vars['mensaje']) ? $get_vars['mensaje'] : 'Sin mensaje';
        $color = isset($get_vars['color']) ? $get_vars['color'] : 'Sin color';
    }
}

// Escapar datos para evitar XSS
$nombre = htmlspecialchars($nombre, ENT_QUOTES, 'UTF-8');
$email = htmlspecialchars($email, ENT_QUOTES, 'UTF-8');
$mensaje = htmlspecialchars($mensaje, ENT_QUOTES, 'UTF-8');
$color = htmlspecialchars($color, ENT_QUOTES, 'UTF-8');
?>
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<title>Resultado del Formulario - PHP</title>
<style>
body {
  background: linear-gradient(135deg, #9b59b6, #8e44ad);
  font-family: Arial, sans-serif;
  color: white;
  text-align: center;
  padding: 50px;
  margin: 0;
}
.card {
  background: rgba(255,255,255,0.1);
  border-radius: 15px;
  padding: 30px;
  max-width: 600px;
  margin: auto;
  backdrop-filter: blur(10px);
  box-shadow: 0 8px 32px rgba(0,0,0,0.3);
}
.header {
  font-size: 2.5em;
  margin-bottom: 30px;
  text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
}
.data-row {
  background: rgba(255,255,255,0.2);
  border-radius: 10px;
  padding: 15px;
  margin: 15px 0;
  text-align: left;
}
.label {
  font-weight: bold;
  color: #ffd700;
}
.back-link {
  display: inline-block;
  background: rgba(255,255,255,0.2);
  color: #fff;
  text-decoration: none;
  padding: 12px 25px;
  border-radius: 25px;
  margin-top: 25px;
  transition: all 0.3s ease;
}
.back-link:hover {
  background: rgba(255,255,255,0.3);
  transform: translateY(-2px);
}
.php-badge {
  background: linear-gradient(45deg, #9b59b6, #8e44ad);
  color: white;
  padding: 5px 12px;
  border-radius: 15px;
  font-size: 0.8em;
  display: inline-block;
  margin-bottom: 20px;
}
</style>
</head>
<body>
<div class="card">
<div class="php-badge">🐘 PHP CGI</div>
<div class="header">📩 Formulario Procesado</div>

<div class="data-row">
  <span class="label">👤 Nombre:</span> <?php echo $nombre; ?>
</div>

<div class="data-row">
  <span class="label">📧 Email:</span> <?php echo $email; ?>
</div>

<div class="data-row">
  <span class="label">💬 Mensaje:</span> <?php echo $mensaje; ?>
</div>

<div class="data-row">
  <span class="label">🎨 Color favorito:</span> <?php echo $color; ?>
</div>

<div class="data-row">
  <span class="label">🕐 Procesado el:</span> <?php echo date('d/m/Y H:i:s'); ?>
</div>

<div class="data-row">
  <span class="label">🌐 Método:</span> <?php echo getenv('REQUEST_METHOD') ?: 'GET'; ?>
</div>

<div class="data-row">
  <span class="label">📊 Content Length:</span> <?php echo getenv('CONTENT_LENGTH') ?: '0'; ?>
</div>

<a href="/cgi.html" class="back-link">⬅ Volver a las pruebas</a>
</div>
</body>
</html>
