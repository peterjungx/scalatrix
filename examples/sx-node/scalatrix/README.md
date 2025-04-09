# Create a scalatrix node module

This shows how to build a scalatrix node module that then can be used in your no-SSR web app.

## Using `npm pack`
In scalatrix home folder
```
make wasm
cd examples/sx-node/scalatrix
cp ../../../build/wasm/scalatrix.js ../../../build/wasm/scalatrix.d.ts lib
npm pack
```

Then in your project folder
```
npm install /path/to/scalatrix-1.x.x.tgz
```

## Using `npm link`
Or alternatively, to work with local copy

```
make wasm
cd examples/sx-node/scalatrix
cp ../../../build/wasm/scalatrix.js ../../../build/wasm/scalatrix.d.ts lib
npm link
```

Then in your project folder
```
npm link scalatrix
```

Note that `npm link` can be finicky.

## Usage from web app

```
import {sx} from 'scalatrix';
import type {AffineTransform, Scale, MOS} from 'scalatrix';
```


