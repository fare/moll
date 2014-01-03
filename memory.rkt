#lang racket

;;; Simple (slow) virtual low-level memory access

(provide +word-size-in-bytes+ +memory-size-in-bytes+
         peek poke peek-unsigned poke-unsigned peek-word poke-word
         peek-u8 poke-u8 peek-u16 poke-u16 peek-u32 poke-u32 peek-u64 poke-u64
         peek-bytes poke-bytes
         peek-string/latin-1 poke-string/latin-1 peek-string/utf-8 poke-string/utf-8)

(define +word-size-in-bytes+ 8)
(define +memory-size-in-bytes+ (expt 2 15)) ; 32KiB

(define *memory* (make-bytes +memory-size-in-bytes+ 0))


(define (peek address) (bytes-ref *memory* address))
(define (poke address value) (bytes-set! *memory* address value))

(define (peek-unsigned length address)
  (do ((address (+ address length -1) (- address 1))
       (length length (- length 1))
       (sum 0))
    ((zero? length) sum)
    (set! sum (+ (arithmetic-shift sum 8) (peek address)))))

(define (poke-unsigned length address value)
  (do ((address address (+ address 1))
       (length length (- length 1))
       (shift 0 (+ 8 shift)))
    ((zero? length))
    (poke address (bitwise-and value 255))
    (set! value (arithmetic-shift value -8))))

(define (peek-u8 address) (peek address))
(define (poke-u8 address value) (poke address value))
(define (peek-u16 address) (peek-unsigned 2 address))
(define (poke-u16 address value) (poke-unsigned 2 address value))
(define (peek-u32 address) (peek-bytes 4 address))
(define (poke-u32 address value) (poke-unsigned 4 address value))
(define (peek-u64 address) (peek-bytes 8 address))
(define (poke-u64 address value) (poke-unsigned 8 address value))

(define (peek-word address) (peek-unsigned +word-size-in-bytes+ address))
(define (poke-word address value) (poke-unsigned +word-size-in-bytes+ address value))

(define (peek-bytes address length)
  (subbytes *memory* address (+ address length)))
(define (poke-bytes address value)
  (bytes-copy! *memory* address value)
  (bytes-length value))

(define (peek-string/latin-1 address length)
  (bytes->string/latin-1 (peek-bytes address length)))
(define (poke-string/latin-1 address value)
  (poke-bytes address (string->bytes/latin-1 value)))
(define (peek-string/utf-8 address length)
  (bytes->string/utf-8 (peek-bytes address length)))
(define (poke-string/utf-8 address value)
  (poke-bytes address (string->bytes/utf-8 value)))
