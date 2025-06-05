# author: Cyber Assassin
# version: 1.0
# date: 2025-06-04
# description: DLS Victim List Scraper

import requests
from bs4 import BeautifulSoup, NavigableString, Tag
import json
import re

def fetch_html_with_tor(url, tor_proxy="socks5h://127.0.0.1:9050"):

    proxies = {
        'http': tor_proxy,
        'https': tor_proxy
    }
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; rv:109.0) Gecko/20100101 Firefox/115.0'
    }
    try:
        print(f"Attempting to fetch URL: {url} via Tor proxy: {tor_proxy}")
        response = requests.get(url, proxies=proxies, headers=headers, timeout=60) 
        response.raise_for_status() 
        print("Successfully fetched HTML content.")
        return response.text
    except requests.exceptions.RequestException as e:
        print(f"Error fetching URL {url}: {e}")
        if "SOCKSHTTPConnectionPool" in str(e) or "Max retries exceeded with url" in str(e):
            print("This might be due to Tor not running or the proxy not being configured correctly.")
            print(f"Please ensure Tor is running and accessible at {tor_proxy}.")
        elif "404 Client Error" in str(e):
            print("The requested page was not found (404 Error).")
        elif "503 Server Error" in str(e) or "504 Server Error" in str(e):
            print("The server is temporarily unavailable or timed out (503/504 Error).")
        return None

def parse_victim_data(html_content):

    if not html_content:
        print("No HTML content to parse.")
        return []

    soup = BeautifulSoup(html_content, 'html.parser')
    victim_records = []

    victim_blocks = soup.find_all('div', class_='border m-2 p-2')
    print(f"Found {len(victim_blocks)} potential victim blocks.")

    for i, block in enumerate(victim_blocks):
        record = {
            "victim": None,
            "description": None,
            "leaked_files": 0,
            "status": "unknown"
        }

        col_10_div = block.find('div', class_='col-10')
        if not col_10_div:
            print(f"Skipping block {i+1}: No 'col-10' div found.")
            continue

        victim_name_tag = col_10_div.find('div', class_='m-2 h4')
        if victim_name_tag and victim_name_tag.find('a'):
            record["victim"] = victim_name_tag.find('a').get_text(strip=True)
        else:
            print(f"Skipping block {i+1} (or entry with no name): No victim name found in 'h4' tag.")
            continue 

        print(f"Processing victim: {record['victim']}")

        if victim_name_tag:
            description_div = victim_name_tag.find_next_sibling('div', class_='m-2')
            if description_div and not description_div.find('div', class_='progress'):
                desc_parts = []
                for content in description_div.contents:
                    if isinstance(content, NavigableString):
                        clean_text = str(content).strip()
                        if clean_text:
                            desc_parts.append(clean_text)
                    elif isinstance(content, Tag):
                        if content.name == 'br':
                            if desc_parts and desc_parts[-1] != ' ':
                                desc_parts.append(' ')
                        else:
                            tag_text = content.get_text(separator=' ', strip=True)
                            if tag_text:
                                desc_parts.append(tag_text)
                full_description = ' '.join(desc_parts).strip()
                record["description"] = re.sub(r'\s+', ' ', full_description)

        target_data_div = None
        potential_data_divs = col_10_div.find_all('div', class_='m-2')
        for d_div in potential_data_divs:
            if d_div.find('div', class_='progress'):
                target_data_div = d_div
                break
        
        if target_data_div:
            doc_links_p = target_data_div.find('p')
            sold_div_found_in_p = False
            if doc_links_p:
                sold_div = doc_links_p.find('div', class_='text-center h2 m-2 text-danger')
                if sold_div and "Sold" in sold_div.get_text(strip=True):
                    record["leaked_files"] = 0
                    record["status"] = "sold"
                    sold_div_found_in_p = True
                else:
                    links_in_p = doc_links_p.find_all('a')
                    actual_doc_links_count = 0
                    for link_tag in links_in_p:
                        href = link_tag.get('href', '')
                        if '.onion/' in href and 'tree.txt' not in href:
                            actual_doc_links_count += 1
                    record["leaked_files"] = actual_doc_links_count
            
            if record["status"] != "sold":
                status_p_tags = target_data_div.find_all('p')            
                for status_text_p in reversed(status_p_tags): # Check from last p upwards
                    if "Data Catalog:" not in status_text_p.get_text(): # Avoid the data catalog line
                        status_text = status_text_p.get_text(strip=True)
                        if "All files was uploaded to public access" in status_text:
                            record["status"] = "data posted"
                            break
                        elif "Not sold data was uploaded" in status_text:
                            record["status"] = "data posted"
                            break
                        elif "All data was sold" in status_text:
                            record["status"] = "sold"
                            if record["leaked_files"] > 0 and not sold_div_found_in_p:
                                record["leaked_files"] = 0
                            break
        else:
            print(f"No target data div (with progress bar) found for victim: {record['victim']}")


        if record["victim"]:
            victim_records.append(record)
            print(f"Successfully parsed record for: {record['victim']}")
        else:
            print(f"Skipped a block due to missing victim name after initial check.")


    return victim_records

if __name__ == "__main__":

    target_url = "http://rhysidafaajcoa2s7se3wnv4rddavp44b6p3x6nvoy7csiiugdgj2vqd.onion/archive.php"    
    tor_proxy_address = "socks5h://127.0.0.1:9050" 
    print("Starting the scraping process...")
    html = fetch_html_with_tor(target_url, tor_proxy=tor_proxy_address)

    if html:
        print("\nParsing fetched HTML content...")
        scraped_data = parse_victim_data(html)
        if scraped_data:
            print("\n--- Scraped Data (JSON) ---")
            print(json.dumps(scraped_data, indent=2))
            

        else:
            print("No data was scraped or parsed successfully.")
    else:
        print("Failed to fetch HTML content. Cannot proceed with parsing.")

    print("\nScraping process finished.")
