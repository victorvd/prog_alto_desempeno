# Hadoop MapReduce Inverted Index

En este proyecto se creará un programa en lenguaje Java para crear un índice invertido (Inverted Index) de palabras que aparecen en un conjunto de documentos extraídos de páginas web. Para ello se usará [Hadoop MapReduce](https://hadoop.apache.org/docs/r2.10.0/hadoop-mapreduce-client/hadoop-mapreduce-client-core/MapReduceTutorial.html).

El conjunto de datos contiene subconjuntos con un total de 74 archivos (etiquetas HTML) derivados del proyecto Stanford WebBase (https://ebiquity.umbc.edu/resource/html/id/351).

Se configurará un clúster de Hadoop en modo distribuido sobre el conjunto de datos real. Salvo que se diga lo contrario, las configuraciones son para el maestro y los esclavos.

## Prerrequisitos

En este parte, instalaremos un clúster Hadoop de un tres nodos respaldado por el sistema de archivos distribuido Hadoop (HDFS) en Ubuntu.

### Instalación de Java

El framework Hadoop está escrito en Java.

```shell
user@ubuntu:~$ cd ~

# Update the source list
user@ubuntu:~$ sudo apt-get update

# The OpenJDK project is the default version of Java 
# that is provided from a supported Ubuntu repository.
user@ubuntu:~$ sudo apt-get install default-jdk

user@ubuntu:~$ java -version
java version "1.7.0_201"
OpenJDK Runtime Environment (IcedTea 2.6.17) (7u211-2.6.17-0ubuntu0.1)
OpenJDK Client VM (build 24.201-b00, mixed mode, sharing)
 ```
### Agregar un usuario dedicado de Hadoop

```shell
user@ubuntu:~$ sudo addgroup hadoop
Adding group `hadoop' (GID 1002) ...
Done.

user@ubuntu:~$ sudo adduser --ingroup hadoop hduser
  Adding user `hduser' ...
  Adding new user `hduser' (1001) with group `hadoop' ...
  Creating home directory `/home/hduser' ...
  Copying files from `/etc/skel' ...
  Enter new UNIX password: 
  Retype new UNIX password: 
  passwd: password updated successfully
  Changing the user information for hduser
  Enter the new value, or press ENTER for the default
	  Full Name []: 
	  Room Number []: 
	  Work Phone []: 
	  Home Phone []: 
	  Other []: 
  Is the information correct? [Y/n] Y
```
```shell
sudo usermod -aG hduser
sudo chown hduser:root -R /usr/local/hadoop/
sudo chmod g+rwx -R /usr/local/hadoop/
sudo adduser hduser sudo
```

En el nodo etiquetado como master (maestro) aplicamos además la siguiente configuración:

```shell
user@ubuntu:~$ hostnamectl set-hostname master
```

En los nodos etiquetados como slaves (esclavos) aplicamos además la siguiente configuración:

```shell
user@ubuntu:~$ hostnamectl set-hostname slave1
```
```shell
user@ubuntu:~$ hostnamectl set-hostname slave2
```
### Instalación de SSH

SSH tiene dos componentes principales:

  1. ssh: comando para conectarse a máquinas remotas: clientes.
  2. sshd: daemon que se ejecuta en el servidor y permite a los clientes conectarse al servidor.

SSH está prehabilitado en Linux, pero para iniciar el daemon sshd, primero se debe instalar SSH. Utilice el siguiente comando en el maestro y los esclavos:

```shell
sudo apt-get install ssh
```
Esto instalará SSH en el nodo. Si obtenemos algo similar a lo siguiente, podemos pensar que está configurado correctamente:

```shell
which ssh
  /usr/bin/ssh

which sshd
  /usr/sbin/sshd
```

### Crear y configurar certificados SSH

Hadoop requiere acceso SSH para administrar sus nodos, es decir, máquinas remotas y la máquina local.

Se requiere tener SSH funcionando en los esclavos y configurarlo para permitir la autenticación de clave pública SSH.

Hadoop utiliza SSH (para acceder a sus nodos), lo que normalmente requeriría que el usuario ingresara una contraseña. Sin embargo, este requisito se puede eliminar creando y configurando certificados SSH utilizando los siguientes comandos en el maestro. Si se le solicita un nombre de archivo, se deja en blanco y se presiona la tecla Intro para continuar.

```shell
su hduser
ssh-keygen -t rsa
ssh-copy-id -i ~/.ssh/id_rsa.pub hduser@master
ssh 'hduser@master'
```

```shell
ssh-copy-id -i ~/.ssh/id_rsa.pub hduser@slave1
ssh 'hduser@slave1'
```

```shell
ssh-copy-id -i ~/.ssh/id_rsa.pub hduser@slave2
ssh 'hduser@slave2'
```
## Hadoop en Ubuntu 14.04

### Descarga e instalación

Se puede descargar Hadoop usando el siguiente comando:

```shell
sudo wget -P ~ https://www.apache.org/dyn/closer.cgi/hadoop/common/hadoop-2.10.2/hadoop-2.10.2.tar.gz
```
Ahora en la carpeta donde se descarga este archivo Hadoop se aplica el siguiente comando:

```shell
tar xzf hadoop-3.3.4.tar.gz
```

### Configurar JAVA_HOME:

Se debe encontrar la ruta donde se instaló Java para configurar la variable de entorno JAVA_HOME usando el siguiente comando:

```shell
update-alternatives --config java
  There is only one alternative in link group java (providing /usr/bin/java): /usr/lib/jvm/java-7-openjdk-i386/jre/bin/java
  Nothing to configure.
```

Abrir el archivo hadoop-env.sh en el editor nano.

```shell
nano hadoop/etc/hadoop/hadoop-env.sh
```

Se debe buscar las siguientes líneas, comentar y agregar la siguiente línea:

```shell
# The java implementation to use.
# export JAVA_HOME=${JAVA_HOME}
export JAVA_HOME=/usr/lib/jvm/java-7-openjdk-i386
```

### Cambiar el directorio de la carpeta Hadoop

Se debe mover la instalación de Hadoop al directorio /usr/local/hadoop usando el siguiente comando:

```shell
sudo mv * /usr/local/hadoop
  [sudo] password for hduser: 
  hduser is not in the sudoers file.  This incident will be reported.
```

Si aparece el siguiente mensaje:

```shell
"hduser is not in the sudoers file. This incident will be reported."
```

Se debe iniciar sesión como usuario root y luego agregando hduser a sudo:

```shell
sudo adduser hduser sudo
  [sudo] password for hduser:
  Adding user `hduser' to group `sudo' ...
  Adding user hduser to group sudo
  Done.
```

Ahora, el usuario hduser tiene privilegios de root, ya es posible mover la instalación de Hadoop al directorio /usr/local/hadoop:

```shell
sudo su hduser
sudo mv * /usr/local/hadoop
sudo chown -R hduser:hadoop /usr/local/hadoop
```

### Configuración de Environment File

Abrir el archivo de entorno (environment file) con este comando:

```shell
sudo nano /etc/environment
```

Luego, agregar la siguiente línea al final:

```shell
JAVA_HOME=”/usr/lib/jvm/java-7-openjdk-i386/jre”
```

### Instalar archivos de configuración

Será necesario modificar los siguientes archivos para completar la configuración de Hadoop:

1. ~/.bashrc
2. /usr/local/hadoop/etc/hadoop/hadoop-env.sh
3. /usr/local/hadoop/etc/hadoop/core-site.xml
4. /usr/local/hadoop/etc/hadoop/mapred-site.xml
5. /usr/local/hadoop/etc/hadoop/hdfs-site.xml

Se debe cambiar a usuario hduser.

```shell
sudo su hduser
```
Para editar el archivo .bashrc del directorio de inicio, se requiere la ruta donde se instaló Java para configurar la variable de entorno JAVA_HOME usando el siguiente comando:

```shell
update-alternatives --config java
```

Ahora podemos agregar lo siguiente al final de ~/.bashrc:

```shell
sudo nano ~/.bashrc
```
```shell
#HADOOP VARIABLES START
export JAVA_HOME=/usr/lib/jvm/java-7-openjdk-i386/jre/bin/java
export HADOOP_HOME=/usr/local/hadoop
export HADOOP_INSTALL=$HADOOP_HOME
export PATH=$PATH:$HADOOP_INSTALL/bin
export PATH=$PATH:$HADOOP_INSTALL/sbin
export HADOOP_MAPRED_HOME=$HADOOP_HOME
export HADOOP_COMMON_HOME=$HADOOP_HOME
export HADOOP_HDFS_HOME=$HADOOP_HOME
export HADOOP_CLASSPATH=/usr/lib/jvm/java-7-openjdk-i386/lib/tools.jar
export YARN_HOME=$HADOOP_INSTALL
export HADOOP_COMMON_LIB_NATIVE_DIR=$HADOOP_INSTALL/lib/native
export HADOOP_OPTS="-Djava.library.path=$HADOOP_INSTALL/lib"
#HADOOP VARIABLES END
```
Para aplicar los cambios al entorno, ejecute el siguiente comando:

```shell
source ~/.bashrc
```
Se necesita agregar las direcciones IP de la máquina al archivo de hosts para todos los nodos. Se debe asegurar que los nodos estén en la misma red.

Se puede ejecutar el siguiente comando para obtener la dirección IP de la máquina:

```shell
ip a
```
Para agregar las direcciones IP abrimos el archivos de hosts:

```shell
sudo nano /etc/hosts
```

Se debe escribir las direcciones IP para el nodo maestro y los esclavos como se muestra:

```shell
127.0.0.1	localhost

10.0.2.4	master
10.0.2.5	slave1
10.0.2.6	slave2
```
Abrir el archivo de nombre de host (hostname) en todos los nodos. Cambiar el nombre de las máquinas a master, slave1 y slave2, según corresponda.

```shell
sudo nano /etc/hostname
```

```shell
	master
```

```shell
	slave1
```

```shell
	slave2
```

Se debe reiniciar los dispositivos para aplicar los cambios.

Después de haber configurado el archivo bashrc, podemos sustituir la ruta '/usr/local/hadoop' por '$HADOOP_HOME'. 

El archivo /usr/local/hadoop/etc/hadoop/core-site.xml contiene propiedades de configuración que Hadoop utiliza al iniciarse. Este archivo se puede utilizar para anular la configuración predeterminada con la que comienza Hadoop.

```shell
sudo mkdir -p /app/hadoop/tmp
sudo chown hduser:hadoop /app/hadoop/tmp
```

Abrir el archivo core-site.xml con el siguiente comando:

```shell
sudo nano $HADOOP_HOME/etc/hadoop/core-site.xml
```

Agregue las siguientes configuraciones entre las etiquetas de configuración:

```shell
<configuration>
 <property>
  <name>hadoop.tmp.dir</name>
  <value>/app/hadoop/tmp</value>
  <description>A base for other temporary directories.</description>
 </property>

 <property>
  <name>fs.defaultFS</name>
  <value>hdfs://master:54310</value>
  <description>Nombre del sistema de archivos predeterminado. Un URI cuyo esquema y autoridad determinan la implementación del sistema de archivos. El esquema URI determina la propiedad de configuración (fs.SCHEME.impl) que nombra la clase de implementación del FileSystem. La autoridad del URI se utiliza para determinar el host, puerto, etc. de un sistema de archivos.</description>
 </property>

 <property>
  <name>io.file.buffer.size</name>
  <value>131072</value>
 </property>
</configuration>
```

El archivo /usr/local/hadoop/etc/hadoop/hdfs-site.xml debe configurarse para cada host en el clúster que se está utilizando. Se utiliza para especificar los directorios que se utilizarán como NameNode y DataNode en ese host.

Antes de editar este archivo, necesitamos crear dos directorios que contendrán el nodo de nombre y el nodo de datos para esta instalación de Hadoop. Esto se puede hacer usando los siguientes comandos:

```shell
sudo mkdir -p /usr/local/hadoop_store/hdfs/namenode
sudo mkdir -p /usr/local/hadoop_store/hdfs/datanode
sudo chown -R hduser:hadoop /usr/local/hadoop_store
```

Abrir el archivo hdfs-site.xml con el siguiente comando:

```shell
sudo nano $HADOOP_HOME/etc/hadoop/hdfs-site.xml
```

Se deben realizar las siguientes configuraciones como se muestra:

```shell
<configuration>
    <property>
        <name>dfs.replication</name>
        <value>2</value>
    </property>
    <property>
        <name>dfs.namenode.name.dir</name>
        <value>file:/usr/local/hadoop_store/hdfs/namenode</value>
    </property>
    <property>
        <name>dfs.datanode.data.dir</name>
        <value>file:/usr/local/hadoop_store/hdfs/datanode</value>
    </property>
</configuration>
```

De forma predeterminada, la carpeta /usr/local/hadoop/etc/hadoop/ contiene /usr/local/hadoop/etc/hadoop/mapred-site.xml.template; archivo que debe cambiarse de nombre o copiarse con el nombre mapred-site.xml:

```shell
cp $HADOOP_HOME/etc/hadoop/mapred-site.xml.template $HADOOP_HOME/etc/hadoop/mapred-site.xml
```

El archivo mapred-site.xml se utiliza para especificar qué marco se utiliza para MapReduce. Necesitamos ingresar el siguiente contenido.

```shell
<configuration>
 <property>
  <name>mapreduce.framework.name</name>
  <value>yarn</value>
 </property>
 <property>
  <name>mapred.child.java.opts</name>
  <value>-Djava.security.egd=file:/dev/../dev/urandom</value>
 </property>
</configuration>
```

El archivo Yarn-site.xml contiene los ajustes de configuración relacionados con YARN. Contiene configuraciones para el Node Manager, Resource Manager, Containers, y Application Master. Abrir el archivo usando el siguiente comando:

```shell
sudo nano $HADOOP_HOME/etc/hadoop/yarn-site.xml
```

Agregar las siguientes configuraciones:

```shell
<configuration>
 <property>
    <name>yarn.resourcemanager.hostname</name>
    <value>master</value>
 </property>

  <property>
    <name>yarn.nodemanager.aux-services</name>
    <value>mapreduce_shuffle</value>
  </property>

  <property>
    <name>yarn.nodemanager.aux-services.mapreduce_shuffle.class</name>
    <value>org.apache.hadoop.mapred.ShuffleHandler</value>
  </property>
</configuration>
```

Es posible, para evitar configurar cada archivo en los nodos esclavo, utilizar el siguiente comando en el maestro para copiar los archivos, cambiando el nombre del esclavo según corresponda:

```shell
scp $HADOOP_HOME/etc/hadoop/hdfs-site.xml hduser@slave1:$HADOOP_HOME/etc/hadoop/
scp $HADOOP_HOME/etc/hadoop/core-site.xml hduser@slave1:$HADOOP_HOME/etc/hadoop/
scp $HADOOP_HOME/etc/hadoop/mapred-site.xml hduser@slave1:$HADOOP_HOME/etc/hadoop/
scp $HADOOP_HOME/etc/hadoop/yarn-site.xml hduser@slave1:$HADOOP_HOME/etc/hadoop/
scp $HADOOP_HOME/etc/hadoop/hadoop-env.sh hduser@slave1:$HADOOP_HOME/etc/hadoop/
```

En el nodo master, abrir el archivo workers:

```shell
sudo nano $HADOOP_HOME/etc/hadoop/workers
```

Agregar las siguientes líneas como se muestra:

```shell
slave1
slave2
# localhost
```

También en el nodo master, abrir el archivo slaves:

```shell
sudo nano $HADOOP_HOME/etc/hadoop/slaves
```

Agregar las siguientes líneas:

```shell
slave1
slave2
```

### Formatear el nuevo sistema de archivos Hadoop

Es necesario formatear el sistema de archivos Hadoop para que podamos empezar a utilizarlo. El comando de formato debe emitirse con permiso de escritura ya que crea el directorio actual en la carpeta /usr/local/hadoop_store/hdfs/namenode:

```shell
hdfs namenode -format
```

El comando hdfs namenode -format debe ejecutarse una vez antes de comenzar a usar Hadoop. Si este comando se ejecuta nuevamente después de haber utilizado Hadoop, destruirá todos los datos en el sistema de archivos de Hadoop.

### Iniciando Hadoop

Para iniciar el nodo maestro se puede usar:

```shell
sudo su hduser
start-dfs.sh && start-yarn.sh
```

O también este comando que ya está en desuso (deprecated):

```shell
start-all.sh
```

Para comprobar si realmente está funcionando:

```shell
jps
  3439 NameNode
  3866 ResourceManager
  3661 SecondaryNameNode
  3938 Jps
```

El resultado significa que ahora tenemos una instancia funcional de Hadoop ejecutándose en el maestro.

Otra forma de comprobarlo es usando netstat:

```shell
netstat -plten | grep java
  (Not all processes could be identified, non-owned process info
   will not be shown, you would have to be root to see it all.)
  tcp      0      0 0.0.0.0:50020           0.0.0.0:*           LISTEN      1001      1843372     10605/java      
  tcp      0      0 10.0.2.3:54310         0.0.0.0:*           LISTEN      1001      1841277     10447/java      
  tcp      0      0 0.0.0.0:50090           0.0.0.0:*           LISTEN      1001      1841130     10895/java      
  tcp      0      0 0.0.0.0:50070           0.0.0.0:*           LISTEN      1001      1840196     10447/java      
  tcp      0      0 0.0.0.0:50010           0.0.0.0:*           LISTEN      1001      1841320     10605/java      
  tcp      0      0 0.0.0.0:50075           0.0.0.0:*           LISTEN      1001      1841646     10605/java      
  ...
```

Si la configuración se realizó correctamente, en los nodos esclavos se podrá observar que también se están ejecutando procesos de hadoop:

```shell
jps
  5840 DataNode
  5723 Jps
  6207 NodeManager
```

Se puede verificar si todo funciona correctamente yendo a la dirección 10.0.2.4:50070 desde cualquier nodo, o localhost:50070 desde el maestro. Debe mostrarse 1 nodo activo en el resumen (summary).


```shell
http://10.0.2.4:50070
```

También es posible hacer esta verificación en la dirección 10.0.2.4:8088 (o localhost:8088 en el maestro). Se mostrarán detalles de la red. Debe haber 1 nodo activo.

```shell
http://10.0.2.4:8088
```

Ejecutamos el siguiente comando para detener todos los daemon que se ejecutan en el nodo:

```shell
stop-dfs.sh && stop-yarn.sh
```

O este otro comando que ya está en desuso:

```shell
stop-all.sh
```

Cuando se quiera volver a iniciar esta red, simplemente actualice las direcciones IP en el archivo /etc/hosts (en todos los nodos).

Para iniciar la red simplemente se utiliza el siguiente comando en el nodo maestro:

```shell
start-dfs.sh && start-yarn.sh
```


## Programa Inverted Index

### Configurar y ejecutar trabajo (job) en Hadoop

Este trabajo obtiene archivos de texto de la carpeta "input" como argumentos del Mapper. Al enviar un trabajo de Hadoop y aplicar el paso Reduce, genera un índice invertido.

Para enviar un trabajo de Hadoop, la implementación de MadReduce debe empaquetarse como un archivo jar. Para hacerlo, se va a copiar el archivo InvertedIndex.java a la carpeta raíz y ejecutar los siguientes comandos para compilar InvertedIndex.java y crear un archivo jar.

```shell
hadoop com.sun.tools.javac.Main InvertedIndex.java
jar cf invertedindex.jar InvertedIndex*.class
```

Copiar los archivos de entrada ([link](https://drive.google.com/uc?id=1sYSQwrPvYBc264OdWVntC5Ts4wu6-9nY&export=download)) para este proyecto dentro de la carpeta de entrada HDFS. Ejecutar el siguiente comando para enviar el trabajo, obtener los archivos de entrada de la carpeta de entrada, generar el índice invertido y almacenar su salida en la carpeta de salida:

```shell
hdfs dfs -mkdir -p InvertedIndex/input
hdfs dfs -put /home/hduser/Downloads/DATA/devdatat/fil* InvertedIndex/input
```

```shell
hadoop jar invertedindex.jar InvertedIndex InvertedIndex/input InvertedIndex/output
```

Y finalmente para ver el resultado, se ejecuta el siguiente comando:

```shell
hdfs dfs -cat InvertedIndex/output/part-r-00000
```

Si se quiere volver a ejecutar el programa, se debe eliminar el directorio de salida antes.

```shell
hdfs dfs -rm -r InvertedIndex/output
```
