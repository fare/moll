#lang racket

;;; Simple (slow) virtual low-level memory access

(provide peek-bytes poke-bytes ;; peek-stringz poke-stringz
         peek-unsigned poke-unsigned peek-u8 poke-u8 peek-u16 poke-u16 peek-u32 poke-u32 peek-u64 poke-u64)

(define +memory-size-in-bytes+ (expt 2 16)) ; 64KiB
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
(define (peek-u16 address) (peek-bytes 2 address))
(define (poke-u16 address value) (poke-bytes 2 address value))
(define (peek-u32 address) (peek-bytes 4 address))
(define (poke-u32 address value) (poke-bytes 4 address value))
(define (peek-u64 address) (peek-bytes 8 address))
(define (poke-u64 address value) (poke-bytes 8 address value))

(define (peek-bytes address length) (subbytes *memory* address (+ address length)))
(define (poke-bytes address value) (bytes-copy! *memory* address value))
