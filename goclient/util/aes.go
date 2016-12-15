package util

import (
	"bytes"
	"crypto/aes"
	"robot/cipher"
	// "encoding/base64"
	// "fmt"
)

func AesEncrypt(origData, key []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}
	blockSize := block.BlockSize()
	// fmt.Println("blocksize: ", blockSize)
	// fmt.Println("DATA UNPADDI:", origData)
	// fmt.Println("LEN: ", len(origData))
	origData = PKCS5Padding(origData, blockSize)
	// fmt.Println("DATA PADDING:", origData)
	// fmt.Println("LEN: ", len(origData))
	// origData = ZeroPadding(origData, block.BlockSize())
	blockMode := cipher.NewECBEncrypter(block)
	crypted := make([]byte, len(origData))
	// 根据CryptBlocks方法的说明，如下方式初始化crypted也可以
	// crypted := origData
	blockMode.CryptBlocks(crypted, origData)
	// fmt.Println("DATA ENCRYPT:", crypted)
	// fmt.Println("LEN: ", len(crypted))
	return crypted, nil
}

func AesDecrypt(crypted, key []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}
	// blockSize := block.BlockSize()
	blockMode := cipher.NewECBDecrypter(block)
	origData := make([]byte, len(crypted))
	// origData := crypted
	blockMode.CryptBlocks(origData, crypted)
	// fmt.Printf("%x\n", crypted)
	origData = PKCS5UnPadding(origData)
	// origData = ZeroUnPadding(origData)
	return origData, nil
}

func ZeroPadding(ciphertext []byte, blockSize int) []byte {
	padding := blockSize - len(ciphertext)%blockSize
	padtext := bytes.Repeat([]byte{0}, padding)
	return append(ciphertext, padtext...)
}

func ZeroUnPadding(origData []byte) []byte {
	length := len(origData)
	unpadding := int(origData[length-1])
	return origData[:(length - unpadding)]
}

func PKCS5Padding(ciphertext []byte, blockSize int) []byte {
	padding := blockSize - len(ciphertext)%blockSize
	padtext := bytes.Repeat([]byte{byte(padding)}, padding)
	return append(ciphertext, padtext...)
}

func PKCS5UnPadding(origData []byte) []byte {
	length := len(origData)
	// 去掉最后一个字节 unpadding 次
	unpadding := int(origData[length-1])
	return origData[:(length - unpadding)]
}
