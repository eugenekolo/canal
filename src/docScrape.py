#!/usr/bin/python

from BeautifulSoup import BeautifulSoup
import requests
import os

## CONFIG - Must change these!
OUTPATH = '/home/eugenek/testdir'
URL = 'http://www.opensource.apple.com'
MBYTE=1024*1024

## Globals
html0 = requests.get(URL).text
soup0 = BeautifulSoup(html0)

##  
# Apple's open source home page lists links for versions of OSX
# This downloads every tar file from each of those versions.
#
def main():
	link_names = [link['href'] for link in soup0.findAll('a', href=True)]
	version_links = [x for x in link_names if x.startswith('/release/mac')]
	
	for version_link in version_links:
		version = version_link.split('/')[-2] # Version # in the URL
		html = requests.get(URL+version_link).text
		soup = BeautifulSoup(html)
	
		if not os.path.exists(OUTPATH + '/' + version):
			os.makedirs(OUTPATH + '/' + version)

		## Filter out the tar links from the version page
		link_names = [link['href'] for link in soup.findAll('a', href=True)] # Extract out link names
		tar_links = [x for x in link_names if x.endswith('tar.gz')] # Filter out tar links from link names
		tar_links = [x if x.startswith('http') else URL+x for x in tar_links] # Correct redirects by appending URL
		
		## Attempt downloading every tar
		for tar_link in tar_links:
			file_name = tar_link.split('/')[-1]
			out_name = OUTPATH + '/' + version + '/' + file_name
			r = requests.get(tar_link, stream=True)
	
			if (os.path.isfile(out_name) == True): 
				continue #  TODO(eugenek): This should create another unique dir instead of skipping

			if(r.status_code != requests.codes.ok):
				continue

			fsize = int(r.headers['content-length'])
			print 'Downloading %s (%sMb)' % ( file_name, fsize/MBYTE )
			with open(out_name, 'wb') as fd:
				for chunk in r.iter_content(chunk_size=1024): # chuck size can be larger
					if chunk: # ignore keep-alive requests
						fd.write(chunk)
	
			pass

###############################################################################
# Main ~!
###############################################################################
if __name__ ==  "__main__":
	main()
