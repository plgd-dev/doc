const algoliasearch = require('algoliasearch');
const fs = require('fs');

// Initialize Algolia client
const client = algoliasearch(process.env.APPLICATION_ID, process.env.API_KEY);
const index = client.initIndex('doc');

// Read JSON file
const jsonData = fs.readFileSync(process.argv[1], 'utf8');

// Parse JSON data
const records = JSON.parse(jsonData);

// Clear the existing index
index.clearObjects()
  .then(() => {
    console.log('Existing records cleared');
    // Add or update records in Algolia index
    return index.saveObjects(records, { autoGenerateObjectIDIfNotExist: true });
  })
  .then(({ objectIDs }) => {
    console.log('Records added/updated:', objectIDs);
  })
  .catch(error => {
    console.error('Error adding/updating records:', error);
  });

