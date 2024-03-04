const algoliasearch = require('algoliasearch');
const fs = require('fs');

// Initialize Algolia client
const client = algoliasearch('42D6VHXINQ', '16bcaaf3bbf281af8c223e692457336c');
const index = client.initIndex('doc');

// Read JSON file
const jsonData = fs.readFileSync('algolia_final.json', 'utf8');

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

