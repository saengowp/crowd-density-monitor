FROM node:14-alpine AS build
RUN apk update && apk add --no-cache \
  build-base \
  g++ \
  cairo-dev \
  jpeg-dev \
  pango-dev \
  giflib-dev \
  make \
  python
  
COPY ["package.json", "yarn.lock", "./"]
RUN yarn

COPY . .
RUN yarn build

EXPOSE 3000
CMD ["node", "/dist/index.js"]