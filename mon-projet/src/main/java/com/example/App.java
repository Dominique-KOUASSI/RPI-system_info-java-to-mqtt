package com.example;

import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import java.io.BufferedReader;
import java.io.InputStreamReader;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;


public class App {
    public static void main(String[] args) {

        //String broker = "tcp://192.168.20.247:1883"; 	// Adresse du broker
		String broker = "tcp://192.168.20.221:1883"; 	// Adresse du broker
        String topic = "testTopic"; 					// Topic sur lequel publier le message
        String content = "Hello MQTT from Java!"; 		// Contenu du message
        int qos = 0; 									// Qualité de service (0, 1 ou 2)
        String clientId = "JavaPublisher"; 				// Identifiant unique du client MQTT

        // Identifiants de la connexion
        String username = "ccnb"; 						// Nom d'utilisateur MQTT
        String password = "ccnb"; 						// Mot de passe MQTT

        MemoryPersistence persistence = new MemoryPersistence();

		// Variables pour stocker les informations du RASPBERRY PI
		String memory_usage = "";
		String cpu_usage = "";
		String up_time = "";
		String ip_address = "";
		
		
		String json_data = "";
		
		
		// *** Variables à enregistrer
		/*
        memory_usage = "50%";
        cpu_usage = "30%";
        up_time = "72 hours";
        ip_address = "192.168.1.1";
		*/ 
		// Appel de la méthode pour enregistrer dans la base de données
        // saveDataToDatabase(memory_usage, cpu_usage, up_time, ip_address);
		
		

		
        // execute une commande shell pour obtenir des informations
        try {
            // Créer une instance de ProcessBuilder avec la commande shell
            ProcessBuilder processBuilder = new ProcessBuilder();
			
			// Request for memory usage
            processBuilder.command("bash", "-c", "free -m | awk 'NR==2{printf \"%.2f%%\", $3*100/$2}' | cut -d \"%\" -f 1");  // Remplacez "ls -la" par votre commande
            // Démarrer le processus
            Process process = processBuilder.start();
            // Lire la sortie de la commande
            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            StringBuilder output = new StringBuilder();
            String line;
            while ((line = reader.readLine()) != null) {
                output.append(line).append("\n");
            }
            // Attendre la fin du processus
            int exitCode = process.waitFor();
            System.out.println("Exit code : " + exitCode);
            System.out.println("Résultat de la commande :\n" + output);

			memory_usage = output.toString();

			// Request for cpu usage
            processBuilder.command("bash", "-c", "mpstat | awk '$12 ~ /[0-9.]+/ { print 100 - $12 }'");  // Remplacez "ls -la" par votre commande
            // Démarrer le processus
            process = processBuilder.start();
            // Lire la sortie de la commande
            reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            output = new StringBuilder();
            while ((line = reader.readLine()) != null) {
                output.append(line).append("\n");
            }
            // Attendre la fin du processus
            exitCode = process.waitFor();
            System.out.println("Exit code : " + exitCode);
            System.out.println("Résultat de la commande :\n" + output);			
			
			cpu_usage = output.toString();	
			
			// Request for uptime
            processBuilder.command("bash", "-c", "awk '{print int($1/60)}' /proc/uptime");  // Remplacez "ls -la" par votre commande
            // Démarrer le processus
            process = processBuilder.start();
            // Lire la sortie de la commande
            reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            output = new StringBuilder();
            while ((line = reader.readLine()) != null) {
                output.append(line).append("\n");
            }
            // Attendre la fin du processus
            exitCode = process.waitFor();
            System.out.println("Exit code : " + exitCode);
            System.out.println("Résultat de la commande :\n" + output);			
			
			up_time = output.toString();			
			
			// Request for IP
            processBuilder.command("bash", "-c", "hostname -I | awk '{print $1}'");  // Remplacez "ls -la" par votre commande
            // Démarrer le processus
            process = processBuilder.start();
            // Lire la sortie de la commande
            reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            output = new StringBuilder();
            while ((line = reader.readLine()) != null) {
                output.append(line).append("\n");
            }
            // Attendre la fin du processus
            exitCode = process.waitFor();
            System.out.println("Exit code : " + exitCode);
            System.out.println("Résultat de la commande :\n" + output);			
			
			ip_address = output.toString();

			json_data = "{\"memory_usage\":" + memory_usage + ", \"cpu_usage\":" + cpu_usage + ", \"up_time\":" + up_time + ", \"ip_address\":\"" + ip_address + "\"}";	
			
			// Affichage du Json
			System.out.println("json_data : " + json_data);
			
			
        } catch (Exception e) {
            e.printStackTrace();
        }
		
		// Save into database
		saveDataToDatabase(memory_usage.replace("\n", ""), cpu_usage.replace("\n", ""), up_time.replace("\n", ""), ip_address.replace("\n", ""));

		// Send to Broker 
		content = json_data.replace("\n", "");
		
        try {
            // Création d'un client MQTT
            MqttClient mqttClient = new MqttClient(broker, clientId, persistence);

            // Configuration des options de connexion
            MqttConnectOptions connOpts = new MqttConnectOptions();
            connOpts.setUserName(username);
            connOpts.setPassword(password.toCharArray());
            connOpts.setCleanSession(true);

            // Connexion au broker avec les identifiants
            mqttClient.connect(connOpts);
            System.out.println("Connecté au broker avec authentification : " + broker);

            // Création et publication du message
            MqttMessage message = new MqttMessage(content.getBytes());
            message.setQos(qos);
            mqttClient.publish(topic, message);

            System.out.println("Message publié : " + content);

            // Déconnexion
            mqttClient.disconnect();
            System.out.println("Déconnecté du broker");

            mqttClient.close();

        } catch (MqttException e) {
            System.out.println("Erreur de publication : " + e.getMessage());
            e.printStackTrace();
        }
		
		
		
		
		

		
		
    }

    public static void saveDataToDatabase(String memoryUsage, String cpuUsage, String upTime, String ipAddress) {
        String jdbcUrl = "jdbc:mysql://localhost:3306/ccnb";
        String username = "ccnb";
        String password = "ccnb";
        String sql = "INSERT INTO system_info (memory_usage, cpu_usage, up_time, ip_address) VALUES (?, ?, ?, ?)";

        try (Connection connection = DriverManager.getConnection(jdbcUrl, username, password);
             PreparedStatement statement = connection.prepareStatement(sql)) {

            statement.setString(1, memoryUsage);
            statement.setString(2, cpuUsage);
            statement.setString(3, upTime);
            statement.setString(4, ipAddress);

            int rowsInserted = statement.executeUpdate();
            if (rowsInserted > 0) {
                System.out.println("Données insérées avec succès !");
            }
        } catch (SQLException e) {
            System.out.println("Erreur lors de la connexion ou de l'insertion : " + e.getMessage());
        }
    }
	
	
}
