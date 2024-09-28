package main

import (
	"encoding/json"
	"fmt"
	"log"
	"os"

	"github.com/algolia/algoliasearch-client-go/v4/algolia/search"
)

func main() {
	// Retrieve environment variables
	applicationID := os.Getenv("APPLICATION_ID")
	if applicationID == "" {
		log.Fatal("APPLICATION_ID environment variable is required")
	}
	apiKey := os.Getenv("API_KEY")
	if apiKey == "" {
		log.Fatal("API_KEY environment variable is required")
	}

	// Initialize Algolia client
	client, err := search.NewClient(applicationID, apiKey)
	if err != nil {
		log.Fatalf("Error initializing Algolia client: %v", err)
	}
	//index := client.InitIndex("doc")

	// Print command-line arguments
	for i, arg := range os.Args {
		fmt.Printf("args %d: %s\n", i, arg)
	}

	// Check if file path is provided
	if len(os.Args) < 2 {
		log.Fatal("Please provide the path to the JSON file")
	}

	// Read JSON file
	filePath := os.Args[1]
	jsonData, err := os.ReadFile(filePath)
	if err != nil {
		log.Fatalf("Error reading JSON file: %v", err)
	}

	// Parse JSON data
	var records []map[string]interface{}
	if err := json.Unmarshal(jsonData, &records); err != nil {
		log.Fatalf("Error parsing JSON data: %v", err)
	}

	// Clear the existing index
	if _, err := client.ClearObjects(client.NewApiClearObjectsRequest("doc")); err != nil {
		log.Fatalf("Error clearing index: %v", err)
	}
	fmt.Println("Existing records cleared")

	// Add or update records in Algolia index
	res, err := client.SaveObjects("doc", records, search.WithCreateIfNotExists(true))
	if err != nil {
		log.Fatalf("Error adding/updating records: %v", err)
	}

	fmt.Println("Records added/updated:", res)
}
