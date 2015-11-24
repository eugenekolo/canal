from BeautifulSoup import BeautifulSoup
import requests
import os

# point to output directory
outpath = '/home/ah/Desktop/EC521/Project/Apple_OpenSrc'
url = 'http://www.opensource.apple.com'
mbyte=1024*1024

html0 = requests.get(url).text
soup0 = BeautifulSoup(html0)

# scrape base page for OSversions
for name0 in soup0.findAll('a', href=True):
	
	link0 = name0['href']
	if( link0.startswith('/release/mac') ):

		html = requests.get(url + link0).text
		soup = BeautifulSoup(html)

		version = link0.split('/')[-2]

		print(version)

		# create folder for OSversion
		if not os.path.exists('Apple_OpenSrc' + '/' + version):
			os.makedirs('Apple_OpenSrc' + '/' + version)

		# scrape OSver page for .tar files
		for name in soup.findAll('a', href=True):
			thislink = name['href']

			# skip all links except tar file downloads links
			if( thislink.endswith('tar.gz') ):

				# some links are redirects to other websites...
				if ( thislink.startswith('http') ):
					zipurl = thislink
				else:
					zipurl = url + thislink

				fileName = zipurl.split('/')[-1]
				outfname = outpath + '/' + version + '/' + fileName

				# skip package if file already exists
				if (os.path.isfile(outfname) == False):
					r = requests.get(zipurl, stream=True)
			#				print(r.status_code)
					if( r.status_code == requests.codes.ok ) :
						fsize = int(r.headers['content-length'])
						print 'Downloading %s (%sMb)' % ( fileName, fsize/mbyte )
						with open(outfname, 'wb') as fd:
							for chunk in r.iter_content(chunk_size=1024): # chuck size can be larger
								if chunk: # ignore keep-alive requests
									fd.write(chunk)
							fd.close()
