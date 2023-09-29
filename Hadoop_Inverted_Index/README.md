# Hadoop MapReduce Inverted Index

En este proyecto se creará un programa en lenguaje Java para crear un índice invertido (Inverted Index) de palabras que aparecen en un conjunto de documentos extraídos de páginas web. Para ello se usará [Hadoop MapReduce](https://hadoop.apache.org/docs/r2.10.0/hadoop-mapreduce-client/hadoop-mapreduce-client-core/MapReduceTutorial.html).

El conjunto de datos contiene subconjuntos con un total de 408 archivos (etiquetas HTML) derivados del proyecto Stanford WebBase (https://ebiquity.umbc.edu/resource/ html/id/351).

Primero se configurará un clúster de Hadoop en modo distribuido sobre el conjunto de datos real.

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

## Configurar JAVA_HOME:

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

Se debe buscar las siguientes líneas, comentar y agregar la siguiente línea

```shell
# The java implementation to use.
# export JAVA_HOME=${JAVA_HOME}
export JAVA_HOME=/usr/lib/jvm/java-7-openjdk-i386
```

## Cambiar el directorio de la carpeta Hadoop

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

Este error se puede resolver iniciando sesión como usuario root y luego agregando hduser a sudo:

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

## Configuración de Environment File

Abrir el archivo de entorno (environment file) con este comando:

```shell
sudo nano /etc/environment
```

Luego, agregar la siguiente línea al final:

```shell
JAVA_HOME=”/usr/lib/jvm/java-7-openjdk-i386/jre”
```






Then try the following command:
 ```shell
$ bin/hadoop
 ```
This should display the usage documentation for the Hadoop script. Now you are ready to start your Hadoop cluster in the standalone mode.

### Setup and run a simple Hadoop job

This simple Hadoop job, gets two text files from the "input" folder as the arguments of the Mapper.
```shell
#file01
5722018411	Hello World Bye World
```
```shell
#file02
6722018415	Hello Hadoop Goodbye Hadoop
```
And by submitting a Hadoop job and applying Reduce step, it generates an inverted index as below:
```shell
bye	5722018411:1 
goodbye	6722018415:1 
hadoop	6722018415:2 
hello	5722018411:1 6722018415:1 
world	5722018411:2
```
To submit a Hadoop job, the MadReduce implementation should be packaged as a `jar` file. To do so, copy the `InvertedIndex.java` file of this project to the Hadoop's distribution root folder and while you are still there run the following commands to compile `InvertedIndex.java` and create a `jar` file.
```shell
$ bin/hadoop com.sun.tools.javac.Main InvertedIndex.java
$ jar cf invertedindex.jar InvertedIndex*.class
```
Copy `input/file01` and `input/file02` of this project and place them inside the `input` folder of the Hadoop distrubution folder. While you are still there, run the following command to submit the job, get the input files from `input` folder, generate the inverted index and store its output in the `output` folder:
```shell
$ bin/hadoop jar invertedindex.jar InvertedIndex input output
```
And finally to see the output, run the below command:
```shell
$ bin/hadoop dfs -cat output/part-r-00000
```
## Fully-Distributed Mode

In this section we create a cluster with 3 worker nodes on Google Dataproc and run `invertedindex.jar` job on the actual data set. 

### Google Cloud Platform Setup

First we need an account on [Google Cloud Platform](https://cloud.google.com/). You can sign up for a trial with a $300 free credits if you don't have one already.

#### Setting up Your Initial Machine
In the Google Cloud Console either create a new project or select an existing one. For this exercise we will use Dataproc. Using Dataproc, we can quickly create a cluster of compute instances running Hadoop. The alternative to Dataproc would be to individually setup each compute node, install Hadoop on it, set up HDFS, set up master node, etc. Dataproc automates this grueling process for us.

#### Creating a Hadoop Cluster on Google Cloud Platform

In Google Cloud Consol, select Dataproc from the navigation list on the left. If this is the first time you’re using Dataproc then you might need to enable Dataproc API first. Clicking on 'Create Cluster' will take you to the cluster configuration section. Give any unique name to your cluster and select a desired zone. You need to create a master and 3 worker nodes. Select the default configuration processors (n1-standard-4 4vCPU 15 GB memory) for master and each member and reduce the storage to 32 GB HDD storage. Change the number of Worker nodes to 3. Leave everything else default and click on 'Create'.

Now that the cluster is setup we’ll have to configure it a little before we can run jobs on it. Select the cluster you just created from the list of clusters under the cloud Dataproc section on your console. Go to the VM Instances tab and click on the SSH button next to the instance with the Master Role.

Clicking on the SSH button will take you to a Command line Interface(CLI) like an xTerm or Terminal. All the commands in the following steps are to be entered in the
CLI. There is no home directory on HDFS for the current user do we’ll have to set this up before proceeding further. (To find out your user name run `whoami`)
```shell
$ hadoop fs -mkdir -p /user/<your username here>
```
#### Set up environment variables
`JAVA_HOME` has already been setup and we don't need to set it up again.

Please note that this step has to be done each time you open a new SSH terminal. For eliminating this step you can also setup this up `JAVA_HOME`, `PATH`, and `HADOOP_CLASSPATH` in the `etc/hadoop/hadoop-env.sh`.
```shell
$ export PATH=${JAVA_HOME}/bin:${PATH}
$ export HADOOP_CLASSPATH=${JAVA_HOME}/lib/tools.jar
```
Now run:
```shell
$ hadoop fs -ls
```
If there is no error this implies that your cluster was successfully set up. If you do encounter an error it’s most likely due to a missing environment variable or user home directory not being set up right.

To ensure that the environment variables are set, run the command `env`.

**NOTE**: Please disable the billing for the cluster when you are not using it. Leaving it running will cost extra credits.
#### Upload data to Google Cloud Storage

Download the dataset from this [link](https://drive.google.com/drive/u/1/folders/1Z4KyalIuddPGVkIm6dUjkpD_FiXyNIcq) and unzip the contents. You will find two folders inside named 'development' and 'full data'. 'development' data can be used for development and testing purposes.

Click on 'Dataproc' in the left navigation menu. Next, locate the address of the default Google cloud storage staging bucket for your cluster.

Go to the storage section in the left navigation bar and select your cluster’s default bucket from the list of buckets. Click on the `Upload Folder` button and upload the `devdata` and `fulldata` folders individually.

#### Submit Hadoop job and generate output
Now we are ready to submit a Hadoop job to run our MapReduce implementation on the actual data. Either use SSH or `nano`/`vi` command to copy or create `InvertedIndex.java` on the master cluster.

Run the following command to create the `jar` file:
```shell
$ hadoop com.sun.tools.javac.Main InvertedIndex.java
$ jar cf invertedindex.jar InvertedIndex*.class
```
Now you have a jar file for your job. You need to place this jar file in the default cloud bucket of your cluster. Just create a folder called JAR on your bucket and upload it to that folder. If you created your jar file on the cluster’s master node itself use the following commands to copy it to the JAR folder. 
```shell
$ hadoop fs -copyFromLocal ./invertedindex.jar
$ hadoop fs -cp ./invertedindex.jar gs://dataproc-69070.../JAR
```
The `gs://dataproc-69070...` part is the default bucket of your cluster. It needs to be prepended by the gs:// to tell the Hadoop environment that it is a bucket and not a regular location on the filesystem.

#### Submitting the Hadoop job to your cluster

Go to the 'Jobs' section in the left navigation bar of the Dataproc page and click on 'Submit job'. Fill the job parameters as follows:

* **Cluster**: Select the cluster you created
* **Job Type:** Hadoop
* **Jar File:** Full path to the jar file you uploaded earlier to the Google storage bucket. E.g. `gs:///dataproc-69070.../JAR/invertedindex.jar`.
* **Main Class or jar:** The name of the java class you wrote the mapper and reducer in.
* **Arguments:** This takes two arguments.
    * **Input:** Path to the input data you uploaded e.g. `gs:///dataproc-69070.../fulldata`.
    * **Output:** Path to the storage bucket followed by a new folder name e.g. `gs:///dataproc-69070.../fulloutput`. The folder is created during execution. You will get an error if you give the name of an existing folder.
* Leave the rest at their default settings.

Now submit the job. You can see the log while it is running.

**NOTE**: If you encounter a `Java.lang.Interrupted Exception` you can safely ignore it. Your job will still execute.

The output files will be stored in the output folder on the bucket. If you open this folder you’ll notice that the inverted index is in several segments.(Delete the _SUCCESS file in the folder before mergin all the output files). To merge the files and create a final text output run the following commands:
```shell
$ hadoop fs -getmerge gs://dataproc-69070.../fulloutput ./output.txt
$ hadoop fs -copyFromLocal ./output.txt
$ hadoop fs -cp ./output.txt gs://dataproc-69070.../output.txt
```
Now you have successfully created an inverted index of the whole data set. You can use `grep` to see index entries for any apecific word:
```shell
$ grep -w '^peace' output.txt
```
#### Note
If you want to re-compile and submit a new job again you can remove the `.jar`, `.class` ,`.java` and Hadoop files using the command below accordingly.
```shell
$ hadoop fs -rm ./wordcount.jar ./output.txt WordCount*.class
$ hadoop fs -rm gs://dataproc-69070.../JAR/wordcount.jar; gs://dataproc-69070.../output.txt
$ hadoop fs -rm gs://dataproc-69070.../fulloutput/* 
$ hadoop fs -rmdir gs://dataproc-69070.../fulloutput
```
## References
* [Hadoop MapReduce documentation](https://hadoop.apache.org/docs/stable/hadoop-mapreduce-client/hadoop-mapreduce-client-core/MapReduceTutorial.html)
* [Google Dataproc documentation](https://cloud.google.com/dataproc/docs/)

## See Also
* [Google's Map Reduce Engineer Round Table Discussion](https://youtu.be/NXCIItzkn3E)

## License
<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.
