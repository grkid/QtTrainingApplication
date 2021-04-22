#!/usr/bin/env python

# Will split a png cubemap/skymap image produced by blender into 6 separated image files for use in a skybox within unity
# Requires Python Imaging Library > http://www.pythonware.com/products/pil/

# The author takes no responsibility for any damage this script might cause,
# feel free to use, change and or share this script.
# 2013-07, CanOfColliders, m@canofcolliders.com

from PIL import Image
import sys, os
import cv2
import numpy as np

path = os.path.abspath("") + "/";
processed = False

# def processImageNew(path,name):
#     img=cv2.imread(os.path.join(path, name))
#     size=min(img.shape[0],img.shape[1])
#     #图片裁剪为正方形
#     if img.size[0]<img.size[1]:# 更高
#         gap=(img.size[1]-size)/2
# 		startX=0
#     else:
#         gap=(img.size[0]-size)/2

def processImageNew(path,name):
	img=cv2.imread(os.path.join(path, name))
	size=min(img.shape[0],img.shape[1])
	if img.shape[0]<img.shape[1]: # y < x
		gap=int((img.shape[1]-size)/2)
		startX=int(0)
		endX=int(startX+size)
		startY=int(gap)
		endY=int(startY+size)
		img=img[startX:endX,startY:endY]
		# cv2.imshow("1",img)
	else:
		gap = (img.shape[0] - size) / 2
		startX = int(gap)
		endX = int(startX+size)
		startY = int(0)
		endY = int(startY + size)
		img=img[startX:endX,startY:endY]
		# cv2.imshow("1",img)
	# cv2.waitKey()

	# 图片处理
	a=size
	b=int((a*a//5)**0.5)
	h=int((2*a*a)//(5*(a+b)))

	lu1=[0,0]
	ld1=[0,a]
	ru1=[a,0]
	rd1=[a,a]

	lu2=[h,h]
	ld2=[h,a-h]
	ru2=[a-h,h]
	rd2=[a-h,a-h]

	target=np.array([[h,h],[h+b,h],[h+b,h+b],[h,h+b]],dtype = "float32")

	# 中间
	cv2.imwrite(os.path.join(path, addToFilename(name,"_back")), img[h:h+b,h:h+b])

	# 左边
	pointLeft=np.array([lu1,lu2,ld2,ld1],dtype = "float32")
	M=cv2.getPerspectiveTransform(pointLeft,target)
	imgLeft=cv2.warpPerspective(img,M,(img.shape[0],img.shape[1]))
	cv2.imwrite(os.path.join(path, addToFilename(name, "_left")), imgLeft[h:h + b, h:h + b])

	# 右边
	pointRight = np.array([ru2, ru1, rd1, rd2], dtype="float32")
	M = cv2.getPerspectiveTransform(pointRight, target)
	imgRight = cv2.warpPerspective(img, M, (img.shape[0], img.shape[1]))
	cv2.imwrite(os.path.join(path, addToFilename(name, "_right")), imgRight[h:h + b, h:h + b])

	# 上边
	pointUp= np.array([lu1, ru1, ru2, lu2], dtype="float32")
	M = cv2.getPerspectiveTransform(pointUp, target)
	imgUp= cv2.warpPerspective(img, M, (img.shape[0], img.shape[1]))
	cv2.imwrite(os.path.join(path, addToFilename(name, "_up")), imgUp[h:h + b, h:h + b])

	# 下边
	pointDown = np.array([ld2, rd2, rd1, ld1], dtype="float32")
	M = cv2.getPerspectiveTransform(pointDown, target)
	imgDown = cv2.warpPerspective(img, M, (img.shape[0], img.shape[1]))
	cv2.imwrite(os.path.join(path, addToFilename(name, "_down")), imgDown[h:h + b, h:h + b])


	# 插值图
	imgLeft=imgLeft[h:h + b, h:h + b].astype(int)
	imgRight=imgRight[h:h + b, h:h + b].astype(int)
	imgUp=imgUp[h:h + b, h:h + b].astype(int)
	imgDown=imgDown[h:h + b, h:h + b].astype(int)

	imgFront=np.zeros([b,b,img.shape[2]])

	for i in range(b):
		for j in range(b):
			rL=imgLeft[i,b-j-1]
			rR=imgRight[i,b-j-1]
			rU=imgUp[b-i-1,j]
			rD=imgDown[b-i-1,j]

			dL=i
			dR=b-i-1
			dU=j
			dD=b-j-1

			sum=dR*rL+dL*rR+dU*rD+dD*rU
			sum=(sum/(2*b-2)).astype(int)

			imgFront[i,j]=sum

	cv2.imwrite(os.path.join(path, addToFilename(name, "_front")), imgFront[0: b, 0: b])
	

def processImage(path, name):
	img = Image.open(os.path.join(path, name))
	size = img.size[1] / 3 # splits the width of the image by 3, expecting the 3x2 layout blender produces.
	splitAndSave(img,2*size, size, size, addToFilename(name, "_right")) #right
	splitAndSave(img, size, size, size, addToFilename(name, "_back")) # right
	splitAndSave(img, 0, size, size, addToFilename(name, "_left")) #right
	splitAndSave(img, size, 0, size, addToFilename(name, "_up"))
	splitAndSave(img, size, 2*size, size, addToFilename(name, "_down"))
	splitAndSave(img, size * 3, size, size, addToFilename(name, "_front")) #right

def addToFilename(name, add):
	name = name.split('.')
	return name[0] + add + "." + name[1]

def splitAndSave(img, startX, startY, size, name):
	area = (startX, startY, startX + size, startY + size)
	saveImage(img.crop(area), path, name)

def saveImage(img, path, name):
	try:
		img.save(os.path.join(path, name))
	except:
		print ("*   ERROR: Could not convert image.")
		pass

processImageNew("./","skybox_img.jpg")