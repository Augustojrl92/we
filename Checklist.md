Checklist para revisión del proyecto webserv

1. Estructura y organización del repositorio
 El repositorio contiene todos los archivos necesarios (código fuente, Makefile, archivos de configuración, páginas de error, etc.)
 Los archivos tienen nombres correctos y corresponden a sus funcionalidades (ej. código fuente en archivos .cpp/.h, configuraciones en archivos específicos, etc.)
 Existe un Makefile que cumple con los requisitos: reglas all, clean, fclean, re, y no realiza relinking innecesario.
 La compilación con c++ funciona con los flags -Wall -Wextra -Werror -std=c++98.
2. Cumplimiento de requisitos técnicos
 El servidor es no bloqueante, utiliza solo un poll() (o equivalente) para manejar toda la I/O.
 Todo acceso a archivos o sockets pasa siempre por poll().
 El servidor mantiene su operatividad en todo momento, incluso en caso de errores o desconexiones.
 El servidor puede atender múltiples puertos y responder con diferentes contenidos.
 Implementa los métodos HTTP requeridos: GET, POST, DELETE.
 Soporta al menos un CGI (ejemplo: PHP, Python) y ejecuta scripts correctamente en el directorio apropiado.
 La configuración del servidor es leída desde un archivo de configuración proporcionado (como argumento o en ruta por defecto).
 El servidor puede servir contenido estático completo (website estático).
 Los clientes pueden subir archivos.
 El servidor es compatible con navegadores estándar y responde correctamente a las cabeceras y respuestas HTTP.
 Los códigos de estado HTTP son correctos y los mensajes de error predeterminados se muestran cuando no hay páginas específicas.
3. Seguridad, estabilidad y resiliencia
 El servidor no hace fork() a menos que sea para CGI.
 No se realizan operaciones de read() o write() sin pasar por poll().
 El servidor nunca se bloquea o se cuelga en operaciones de I/O.
 Se han realizado pruebas de estrés para verificar disponibilidad continua.
 El servidor puede manejar solicitudes chunked y EOF de manera correcta.
 Se verifican correctamente las variables de entorno para CGI.
 Responde rápidamente sin operaciones que puedan colgar indefinidamente.
4. Funcionalidades adicionales y requisitos
 Incluye páginas de errores predeterminadas si no se proporcionan custom.
 Es posible modificar una pequeña parte del código en evaluación (ejemplo: cambiar un comportamiento, agregar una funcionalidad sencilla).
 La estructura del código permite facilitar esas modificaciones (modularidad, buen diseño).
 Se han realizado pruebas de envío y recepción de solicitudes HTTP con telnet y NGINX.
 La comunicación está basada en HTTP/1.0 o superior, con posibles referencias a HTTP/1.1.
5. Documentación
 Se incluyen instrucciones para compilar y ejecutar el servidor.
 Se dispone de archivos de configuración de prueba.
 Se dispone de páginas de error predeterminadas.
 El código está documentado, especialmente en funciones clave o complejas.