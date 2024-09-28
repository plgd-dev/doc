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
	return "my string representation"
}

func (i *arrayFlags) Set(value string) error {
	*i = append(*i, value)
	return nil
}

// WriteTo writes v to writer.
func WriteTo(w io.Writer, v interface{}) error {
	var h codec.JsonHandle
	h.BasicHandle.Canonical = true
	err := codec.NewEncoder(w, &h).Encode(v)
	if err != nil {
		return fmt.Errorf("JSON encoder failed: %w", err)
	}
	return nil
}

// ReadFrom reads and stores the result in v.
func ReadFrom(w io.Reader, v interface{}) error {
	var h codec.JsonHandle
	err := codec.NewDecoder(w, &h).Decode(v)
	if err != nil {
		return fmt.Errorf("JSON decoder failed: %w", err)
	}
	return nil
}

func getParentID(objectID string) string {
	return filepath.Dir(objectID)
}

func main() {
	rootObjectIDRegex := flag.String("root-object-id", "[a-z0-9A-Z]+/docs$", "Root object ID for the adjust title - if empty, no adjustment is made, if set the string will be used as separator")
	adjustTitle := flag.String("adjust-title", " / ", "Adjust title - if empty, no adjustment is made, if set the string will be used as separator")
	trimPrefixesInURI := arrayFlags{"en/"}
	flag.Var(&trimPrefixesInURI, "trim-prefixes-in-uri", "Remove the given prefixes in the URI")
	flag.Parse()

	var data []map[string]interface{}
	err := ReadFrom(os.Stdin, &data)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
	var regexpRootObjectID *regexp.Regexp
	if *rootObjectIDRegex != "" && *adjustTitle != "" {
		regexpRootObjectID = regexp.MustCompile(*rootObjectIDRegex)
	}
	mapping := make(map[string]map[string]interface{})
	for _, item := range data {
		objectID := item["objectID"].(string)
		mapping[objectID] = item
	}
	for idx, item := range data {
		for _, prefix := range trimPrefixesInURI {
			item["uri"] = strings.TrimPrefix(item["uri"].(string), prefix)
		}
		if regexpRootObjectID != nil {
			parentID := getParentID(item["objectID"].(string))
			for {
				if regexpRootObjectID.Match([]byte(parentID)) {
					break
				}
				parent, ok := mapping[parentID]
				if !ok {
					break
				}
				item["title"] = fmt.Sprintf("%v%s%v", parent["title"], *adjustTitle, item["title"])
				parentID = getParentID(parentID)
			}
		}
		data[idx] = item
	}
	newData := make([]map[string]interface{}, 0, len(data))
	for _, item := range data {
		if len(item["content"].(string)) > 0 {
			newData = append(newData, item)
		}
	}
	err = WriteTo(os.Stdout, newData)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}

}
