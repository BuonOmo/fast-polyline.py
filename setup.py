from distutils.core import setup, Extension

def main():
    setup(name="fast_polyline",
          version="1.0.0",
          description="Fast & Easy Google Polylines",
          author="Ulysse Buonomo",
          license="MIT",
          author_email="buonomo.ulysse@gmail.com",
          ext_modules=[Extension("fast_polyline", sources=["src/polyline.c"])])

if __name__ == "__main__":
    main()
