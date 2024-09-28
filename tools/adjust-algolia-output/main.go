package main

import (
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"regexp"
	"strings"

	"github.com/ugorji/go/codec"
)

type arrayFlags []string

func (i *arrayFlags) String() string {
	return strings.Join(*i, ", ")
}

func (i *arrayFlags) Set(value string) error {
	*i = append(*i, value)
	return nil
}

// JSON encoding and decoding helpers
func writeJSON(w io.Writer, v interface{}) error {
	var h codec.JsonHandle
	h.BasicHandle.Canonical = true
	return codec.NewEncoder(w, &h).Encode(v)
}

func readJSON(r io.Reader, v interface{}) error {
	var h codec.JsonHandle
	return codec.NewDecoder(r, &h).Decode(v)
}

// getParentID returns the parent directory of the objectID
func getParentID(objectID string) string {
	return filepath.Dir(objectID)
}

// adjustURIs removes prefixes from the "uri" field in each item
func adjustURIs(data []map[string]interface{}, prefixes arrayFlags) {
	for _, item := range data {
		for _, prefix := range prefixes {
			item["uri"] = strings.TrimPrefix(item["uri"].(string), prefix)
		}
	}
}

// adjustTitles recursively adjusts the title field for objects that match the given regex
func adjustTitles(data []map[string]interface{}, regex *regexp.Regexp, adjustTitle string, mapping map[string]map[string]interface{}) {
	for _, item := range data {
		if regex == nil {
			continue
		}
		parentID := getParentID(item["objectID"].(string))
		for {
			if regex.MatchString(parentID) {
				break
			}
			parent, exists := mapping[parentID]
			if !exists {
				break
			}
			item["title"] = fmt.Sprintf("%v%s%v", parent["title"], adjustTitle, item["title"])
			parentID = getParentID(parentID)
		}
	}
}

// filterNonEmptyContent filters items that have non-empty "content"
func filterNonEmptyContent(data []map[string]interface{}) []map[string]interface{} {
	filtered := []map[string]interface{}{}
	for _, item := range data {
		if len(item["content"].(string)) > 0 {
			filtered = append(filtered, item)
		}
	}
	return filtered
}

func main() {
	// Parse flags
	rootObjectIDRegex := flag.String("root-object-id", "[a-z0-9A-Z]+/docs$", "Regex to match root object IDs for title adjustment")
	adjustTitle := flag.String("adjust-title", " / ", "String to use as separator when adjusting titles")
	trimPrefixesInURI := arrayFlags{"en/"}
	flag.Var(&trimPrefixesInURI, "trim-prefixes-in-uri", "Prefixes to remove from URI")
	flag.Parse()

	// Read input data
	var data []map[string]interface{}
	if err := readJSON(os.Stdin, &data); err != nil {
		fmt.Fprintf(os.Stderr, "Error reading JSON: %v\n", err)
		os.Exit(1)
	}

	// Create regex if needed
	var regex *regexp.Regexp
	if *rootObjectIDRegex != "" && *adjustTitle != "" {
		regex = regexp.MustCompile(*rootObjectIDRegex)
	}

	// Create objectID to item map
	mapping := make(map[string]map[string]interface{})
	for _, item := range data {
		mapping[item["objectID"].(string)] = item
	}

	// Adjust URIs and titles
	adjustURIs(data, trimPrefixesInURI)
	adjustTitles(data, regex, *adjustTitle, mapping)

	// Filter items with non-empty content
	filteredData := filterNonEmptyContent(data)

	// Write output
	if err := writeJSON(os.Stdout, filteredData); err != nil {
		fmt.Fprintf(os.Stderr, "Error writing JSON: %v\n", err)
		os.Exit(1)
	}
}
