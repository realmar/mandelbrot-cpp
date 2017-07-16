package main

import (
	"math/cmplx"
	"image"
	"image/color"
	"image/png"
	"os"
	"math"
)

const width = 1000
const height = 1000

var mapX [2]float64 = [2]float64{-2.5, 1}
var mapY [2]float64 = [2]float64{-1, 1}

func map_pixel(p float64, m [2]float64, origWith float64) float64 {
	return p / origWith * math.Abs(m[0] - m[1]) + m[0]
}

func mandelbrot_f(z complex128, c complex128) complex128 {
	return cmplx.Pow(2, z) + c
}

func do_mandel(x int, y int, m *image.RGBA, imgPos *int) {
	c := complex(map_pixel(float64(x), mapX, width), map_pixel(float64(y), mapY, height))

	var z complex128 = complex(0, 0)

	for i := 0; i < 400; i++ {
		z = mandelbrot_f(z, c)

		if cmplx.IsNaN(z) {
			break
		}
	}

	if cmplx.IsNaN(z) {
		m.Set(x, y, color.RGBA{255, 255, 255, 255})
	}else{
		m.Set(x, y, color.RGBA{0, 0, 0, 0})
	}

	*imgPos++
}

func main() {
	imgPos := 0
	m := image.NewRGBA(image.Rect(0, 0, width, height))

	for x := 0; x < width; x ++ {
		for y := 0; y < height; y ++ {
			go do_mandel(x, y, m, &imgPos)
		}
	}

	img, _ := os.Create("new.jpg")
	defer img.Close()
	png.Encode(img, m)
}